//
// Created by brand on 6/3/2025.
//

#include "keela-widgets/cameramanager.h"

#include <arv.h>
#include <keela-pipeline/utils.h>
#include <spdlog/spdlog.h>

#include <ctime>
#include <filesystem>
#include <sstream>
#include <stdexcept>

#include "keela-pipeline/recordbin.h"
#include "keela-widgets/plugin_utils.h"

Keela::CameraManager::CameraManager(guint id, bool split_streams)
    : Bin("camera_" + std::to_string(id)), camera(Keela::get_video_source_name()) {
	try {
		spdlog::info("Creating camera manager {}", id);
		this->id = id;

		Bin camera_stream_bin = static_cast<Bin &>(*camera_stream_even);

		// Add all elements to the bin
		// @todo: can we get rid of this caps_filter?
		// I think we can just modify the source and the downstream elements will renegotiate
		add_elements(camera, caps_filter, transform, tee_main, camera_stream_bin);

		// Link main pipeline: camera -> capsfilter -> transform -> main_tee ->
		// camera_stream_even
		element_link_many(camera, caps_filter, transform, tee_main, camera_stream_bin);

		if(split_streams) {
			add_odd_camera_stream();
		}

		// Set up camera control via aravissrc and AravisCamera
		GstElement *camera_element = static_cast<GstElement *>(camera);

		// Only initialize aravis_controller if the camera is an aravissrc
		GstElementFactory *factory = gst_element_get_factory(camera_element);
		const gchar *factory_name = gst_plugin_feature_get_name(factory);
		if(g_strcmp0(factory_name, "aravissrc") == 0) {
			aravis_controller = std::make_unique<AravisController>(camera_element);
			spdlog::info("Initialized AravisController for camera {}", id);
		} else {
			spdlog::info("Camera {} is not an aravissrc ({}), skipping AravisController initialization", id,
			             factory_name);
		}

		// Set up frame splitting if enabled
		this->split_streams = split_streams;
		if(split_streams) {
			install_frame_splitting_probes();
		}

		spdlog::info("Created camera manager {}", id);
	} catch(const std::exception &e) {
		std::stringstream ss;

		ss << "camera" << id << ".dot";
		spdlog::error("could not create camera manager: {}\n attempting to dump bin to {}", e.what(), ss.str());
		dump_bin_graph();
		throw;
	}
}

Keela::CameraManager::~CameraManager() {
	spdlog::debug(__func__);
}

std::vector<std::string> Keela::CameraManager::get_available_pixel_formats() const {
	if(aravis_controller) {
		return aravis_controller->get_available_pixel_formats();
	} else {
		spdlog::warn("{}: araviscontroller not initialized. returning empty pixel format list", __func__);
		return std::vector<std::string>();
	}
}

void Keela::CameraManager::set_pix_fmt(const std::string &format) {
	if(aravis_controller) {
		aravis_controller->set_pixel_format(format);
		restart_pipeline();
	} else {
		spdlog::warn("{}: no aravis_controller set", __func__);
	}
}

void Keela::CameraManager::set_framerate(double framerate) {
	// @todo: switch this to hardware control, no need for the caps anymore
	spdlog::info("Setting framerate to {}", framerate);
	int numerator = static_cast<int>(framerate * 10);
	// TODO: caps need to be writable
	base_caps = Caps(static_cast<GstCaps *>(base_caps));
	base_caps.set_framerate(numerator, 10);

	// through experimentation, I believe that changes to the original caps
	// reference do not affect the capsfilter
	g_object_set(caps_filter, "caps", static_cast<GstCaps *>(base_caps), nullptr);
}

void Keela::CameraManager::set_experiment_directory(const std::string &path) {
	experiment_directory = path;
}

std::pair<double, double> Keela::CameraManager::get_gain_range() const {
	return aravis_controller->get_gain_range();
}

double Keela::CameraManager::get_gain() const {
	return aravis_controller->get_gain();
}

std::pair<double, double> Keela::CameraManager::get_exposure_time_range() const {
	return aravis_controller->get_exposure_time_range();
}

double Keela::CameraManager::get_exposure_time() const {
	return aravis_controller->get_exposure_time();
}

bool Keela::CameraManager::supports_hardware_binning() const {
	if(aravis_controller == nullptr) {
		return false;
	};
	return aravis_controller->supports_hardware_binning();
}

std::vector<std::string> Keela::CameraManager::get_supported_binning_modes() const {
	return aravis_controller->get_supported_binning_modes();
}

std::pair<std::string, std::string> Keela::CameraManager::get_binning_modes() const {
	return aravis_controller->get_binning_modes();
}

std::tuple<int, int, int, int> Keela::CameraManager::get_binning_bounds() const {
	return aravis_controller->get_binning_bounds();
}

std::pair<int, int> Keela::CameraManager::get_binning_increments() const {
	return aravis_controller->get_binning_increments();
}

std::pair<int, int> Keela::CameraManager::get_current_resolution() const {
	return aravis_controller->get_current_resolution();
}

std::pair<int, int> Keela::CameraManager::get_binning_factors() const {
	return aravis_controller->get_binning_factors();
}

void Keela::CameraManager::set_gain(double gain) {
	aravis_controller->set_gain(gain);
}

void Keela::CameraManager::set_exposure_time(double exposure) {
	return aravis_controller->set_exposure_time(exposure);
}

void Keela::CameraManager::set_binning_mode(std::string mode) {
	aravis_controller->set_binning_mode(mode);
	// BinningMode is set with the 'features' property on aravissrc, which doesn't dynamically
	// update the camera settings. We need to restart the pipeline to apply the change.
	restart_pipeline();
}

void Keela::CameraManager::set_binning_factors(int binning_factor) {
	set_binning_factors(binning_factor, binning_factor);
}

void Keela::CameraManager::set_binning_factors(int binning_factor_x, int binning_factor_y) {
	spdlog::info("Setting hardware binning to {}x{}", binning_factor_x, binning_factor_y);

	set_pipeline_state(GST_STATE_NULL);
	aravis_controller->set_binning_factors(binning_factor_x, binning_factor_y);
	set_pipeline_state(GST_STATE_PLAYING);
	// This forces the camera to re-negotiate its caps after the binning change.
	// The aravissrc doesn't reflect width/height changes until the element is set to NULL and back to PLAYING.
	// Without this, we end up with mismatched resolutions between the camera source and the rest of the pipeline.
	restart_pipeline();
}

void Keela::CameraManager::start_recording() {
	std::string suffix = split_streams ? "even" : "";
	IRecordable::start_recording();
	camera_stream_even->start_recording(get_filename(experiment_directory, this->id, suffix));

	if(split_streams) {
		camera_stream_odd->start_recording(get_filename(experiment_directory, this->id, "odd"));
	}
}

void Keela::CameraManager::stop_recording() {
	IRecordable::stop_recording();
	camera_stream_even->stop_recording();

	if(split_streams) {
		camera_stream_odd->stop_recording();
	}
}

GstPadProbeReturn Keela::CameraManager::frame_parity_probe_cb(GstPad *pad, GstPadProbeInfo *info, gpointer user_data) {
	GstBuffer *buffer = GST_PAD_PROBE_INFO_BUFFER(info);
	FrameProbeData *probe_data = static_cast<FrameProbeData *>(user_data);
	int parity = probe_data->parity;

	int frame_number = 0;

	// Some sources will have the frame number in the buffer offset (e.g.
	// videotestsrc)
	if(GST_BUFFER_OFFSET(buffer) != GST_BUFFER_OFFSET_NONE) {
		frame_number = GST_BUFFER_OFFSET(buffer);
	}
	// But others like aravissrc do not set offset, so we fall back to our own
	// per-camera counter
	else {
		frame_number = (*probe_data->counter)++;
	}

	if(frame_number % 2 == parity) {
		return GST_PAD_PROBE_OK;  // Pass the frame
	} else {
		return GST_PAD_PROBE_DROP;  // Drop the frame
	}
}

void Keela::CameraManager::set_frame_splitting(bool enabled) {
	split_streams = enabled;
	spdlog::info("Frame splitting {}", enabled ? "enabled" : "disabled");

	if(enabled) {
		if(camera_stream_odd == nullptr) {
			// re-create the odd camera stream if it was previously ejected
			camera_stream_odd = std::make_shared<CameraStreamBin>("camera_stream_odd");
		}

		// Install the probes now that the pipeline is set up
		install_frame_splitting_probes();
		add_odd_camera_stream();
	} else {
		// Remove probes so the even stream gets all frames
		remove_frame_splitting_probes();

		// Eject the odd CameraStreamBin
		camera_stream_odd->PrepareEject();
		camera_stream_odd->Eject(false);  // false = don't send EOS
		camera_stream_odd = nullptr;      // clear the shared_ptr to allow re-creation later
		spdlog::info("Ejected camera_stream_odd from pipeline");
	}
}

void Keela::CameraManager::install_frame_splitting_probes() {
	spdlog::info("Installing frame splitting probes");

	// Install filtering probes on the sink pads of the even and odd tees
	GstPad *even_sink_pad = gst_element_get_static_pad(camera_stream_even->internal_tee, "sink");
	GstPad *odd_sink_pad = gst_element_get_static_pad(camera_stream_odd->internal_tee, "sink");

	if(even_sink_pad) {
		even_frame_probe_id = gst_pad_add_probe(even_sink_pad, GST_PAD_PROBE_TYPE_BUFFER, frame_parity_probe_cb,
		                                        &even_probe_data, nullptr);
		g_object_unref(even_sink_pad);
		spdlog::info("Installed even frame filter probe");
	}

	if(odd_sink_pad) {
		odd_frame_probe_id =
		    gst_pad_add_probe(odd_sink_pad, GST_PAD_PROBE_TYPE_BUFFER, frame_parity_probe_cb, &odd_probe_data, nullptr);
		g_object_unref(odd_sink_pad);
		spdlog::info("Installed odd frame filter probe");
	}
}

std::string Keela::CameraManager::get_filename(std::string directory, guint cam_id, std::string suffix) {
	time_t timestamp = std::time(nullptr);
	struct tm datetime = *localtime(&timestamp);
	std::stringstream ss;
	ss << std::put_time(&datetime, "%Y%m%d_%H%M%S_");

	if(suffix != "") {
		suffix = "_" + suffix;
	}

	auto path = std::filesystem::path(directory) / (ss.str() + "cam_" + std::to_string(cam_id) + suffix + ".mkv");

	return path.string();
}

void Keela::CameraManager::add_odd_camera_stream() {
	Bin camera_stream_odd_bin = static_cast<Bin &>(*camera_stream_odd);

	add_elements(camera_stream_odd_bin);

	// Sync state with parent if the pipeline is already running
	gboolean sync_result = gst_element_sync_state_with_parent(camera_stream_odd_bin);
	if(!sync_result) {
		spdlog::warn("Failed to sync camera_stream_odd state with parent");
	}

	// Link main tee to camera_stream_odd
	element_link_many(tee_main, camera_stream_odd_bin);
}

void Keela::CameraManager::remove_probe_by_id(gulong &probe_id, GstPad *pad, const std::string &probe_name) {
	gst_pad_remove_probe(pad, probe_id);
	g_object_unref(pad);
	probe_id = 0;
	spdlog::info("Removed {} probe", probe_name);
}

void Keela::CameraManager::remove_frame_splitting_probes() {
	if(even_frame_probe_id != 0) {
		GstPad *even_sink_pad = gst_element_get_static_pad(camera_stream_even->internal_tee, "sink");
		remove_probe_by_id(even_frame_probe_id, even_sink_pad, "even frame filter");
	}
	if(odd_frame_probe_id != 0) {
		GstPad *odd_sink_pad = gst_element_get_static_pad(camera_stream_odd->internal_tee, "sink");
		remove_probe_by_id(odd_frame_probe_id, odd_sink_pad, "odd frame filter");
	}
}

void Keela::CameraManager::set_pipeline_state(GstState state) {
	GstElement *pipeline = GST_ELEMENT(gst_element_get_parent(static_cast<GstElement *>(camera)));
	if(!pipeline) {
		spdlog::error("Failed to get parent pipeline for camera {}", id);
		return;
	}
	GstStateChangeReturn ret = gst_element_set_state(pipeline, state);
	if(ret == GST_STATE_CHANGE_FAILURE) {
		spdlog::error("Failed to set pipeline state");
	}
	// Blocks until state change completes
	gst_element_get_state(pipeline, nullptr, nullptr, GST_CLOCK_TIME_NONE);

	g_object_unref(pipeline);
}

void Keela::CameraManager::restart_pipeline() {
	spdlog::info("Restarting pipeline to apply camera settings");

	set_pipeline_state(GST_STATE_NULL);
	set_pipeline_state(GST_STATE_PLAYING);
}