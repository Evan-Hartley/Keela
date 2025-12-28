//
// Created by brand on 6/3/2025.
//

#include "keela-widgets/cameramanager.h"

#include <arv.h>
#include <keela-pipeline/splitstreambin.h>
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

		// Add all elements to the bin
		// @todo: can we get rid of this caps_filter?
		// I think we can just modify the source and the downstream elements will renegotiate
		add_elements(camera, caps_filter, transform, tee, snapshot);

		// Link main pipeline: camera -> capsfilter -> transform -> main_tee ->
		// camera_stream_even
		element_link_many(camera, caps_filter, transform, tee);
		element_link_many(tee, snapshot);
		// Set up camera control via aravissrc and AravisCamera
		GstElement *camera_element = static_cast<GstElement *>(camera);

		set_frame_splitting(split_streams);

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

void Keela::CameraManager::set_pix_fmt(const std::string &format) {
	// @todo: switch this to hardware, no need for the caps anymore
	spdlog::info("{}: Setting pixel format to {}", __func__, format);
	// create copy of our caps
	base_caps = Caps(static_cast<GstCaps *>(base_caps));
	// apply pixel format
	gst_caps_set_simple(base_caps, "format", G_TYPE_STRING, format.c_str(), nullptr);
	g_object_set(caps_filter, "caps", static_cast<GstCaps *>(base_caps), nullptr);
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

	throw std::runtime_error("Recording not yet implemented");
}

void Keela::CameraManager::stop_recording() {
	throw std::runtime_error("Recording not yet implemented");
}

void Keela::CameraManager::set_frame_splitting(bool split_enabled) {
	split_streams = split_enabled;
	spdlog::info("Frame splitting {}", split_enabled ? "enabled" : "disabled");

	if(stream) {
		stream->Eject(true);
	}
	if(split_enabled) {
		stream = std::make_shared<Keela::SplitStreamBin>();
	} else {
		stream = std::make_shared<Keela::CameraStreamBin>("Single Camera Stream");
	}
	add_elements(*stream);
	element_link_many(tee, *stream);
}
std::vector<std::shared_ptr<Keela::CameraStreamBin>> Keela::CameraManager::get_streams() const {
	std::vector<std::shared_ptr<Keela::CameraStreamBin>> streams;
	if(!is_frame_splitting_enabled()) {
		streams.push_back(std::dynamic_pointer_cast<Keela::CameraStreamBin>(stream));
	} else {
		auto split_stream = std::dynamic_pointer_cast<Keela::SplitStreamBin>(stream);
		streams.push_back(split_stream->even_stream);
		streams.push_back(split_stream->odd_stream);
	}
	return streams;
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