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

Keela::CameraManager::CameraManager(guint id, std::string pix_fmt, bool split_streams) : Bin("camera_" + std::to_string(
            id)), camera(Keela::get_video_source_name()) {
    try {
        spdlog::info("Creating camera manager {}", id);
        this->id = id;

        Bin camera_stream_bin = static_cast<Bin &>(*camera_stream_even);

        // Add all elements to the bin
        add_elements(camera, caps_filter, transform, tee_main, camera_stream_bin);

        // Link main pipeline: camera -> capsfilter -> transform -> main_tee -> camera_stream_even
        element_link_many(camera, caps_filter, transform, tee_main, camera_stream_bin);

        if (split_streams) {
            add_odd_camera_stream();
        }

        // Set up frame splitting if enabled
        this->split_streams = split_streams;
        if (split_streams) {
            install_frame_splitting_probes();
        }

        set_pix_fmt(pix_fmt);
        spdlog::info("Created camera manager {}", id);
    } catch (const std::exception &e) {
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
    spdlog::info("{}: Setting pixel format to {}", __func__, format);
    // create copy of our caps
    base_caps = Caps(static_cast<GstCaps *>(base_caps));
    // apply pixel format
    gst_caps_set_simple(base_caps, "format", G_TYPE_STRING, format.c_str(), nullptr);
    g_object_set(caps_filter, "caps", static_cast<GstCaps *>(base_caps), nullptr);
}

void Keela::CameraManager::set_framerate(double framerate) {
    spdlog::info("Setting framerate to {}", framerate);
    int numerator = static_cast<int>(framerate * 10);
    // TODO: caps need to be writable
    base_caps = Caps(static_cast<GstCaps *>(base_caps));
    base_caps.set_framerate(numerator, 10);

    // through experimentation, I believe that changes to the original caps reference do not affect the capsfilter
    g_object_set(caps_filter, "caps", static_cast<GstCaps *>(base_caps), nullptr);
}

void Keela::CameraManager::set_resolution(const int width, const int height) {
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("width and height must be greater than zero");
    }
    /*
    // create a copy of our current caps
    base_caps = Caps(static_cast<GstCaps *>(base_caps));
    base_caps.set_resolution(width, height);
    g_object_set(caps_filter, "caps", static_cast<GstCaps *>(base_caps), nullptr);
    transform.scale(width, height);*/
    transform.SetResolution(width, height);
}

void Keela::CameraManager::set_experiment_directory(const std::string &path) {
    experiment_directory = path;
}

std::pair<double, double> Keela::CameraManager::get_gain_range() const {
    double min_gain = 0.0;
    double max_gain = 0.0;

    ArvCamera *aravis_camera = get_aravis_camera();
    if (aravis_camera == nullptr) {
        spdlog::warn("Gain control not supported");
        return {min_gain, max_gain};
    }

    spdlog::debug("Querying gain range from camera hardware via ArvCamera object");

    GError *error = nullptr;
    // Query the actual hardware gain limits
    arv_camera_get_gain_bounds(aravis_camera, &min_gain, &max_gain, &error);
    if (error == nullptr) {
        spdlog::info("Queried hardware gain range from camera: {:.1f} to {:.1f} dB", min_gain, max_gain);
    } else {
        spdlog::warn("Error querying gain range from camera: {}", error->message);
        g_error_free(error);
    }

    g_object_unref(aravis_camera);

    return {min_gain, max_gain};
}

std::pair<double, double> Keela::CameraManager::get_exposure_time_range() const {
    double min_exposure = 0.0;
    double max_exposure = 0.0;

    ArvCamera *aravis_camera = get_aravis_camera();
    if (aravis_camera == nullptr) {
        spdlog::warn("Exposure time control not supported");
        return {min_exposure, max_exposure};
    }

    spdlog::debug("Querying exposure time range from camera hardware via ArvCamera object");

    GError *error = nullptr;
    // Query the actual hardware exposure time limits
    arv_camera_get_exposure_time_bounds(aravis_camera, &min_exposure, &max_exposure, &error);
    if (error == nullptr) {
        spdlog::info("Queried hardware exposure time range from camera: {:.1f} to {:.1f} us", min_exposure,
                     max_exposure);
    } else {
        spdlog::warn("Error querying exposure time range from camera: {}", error->message);
        g_error_free(error);
    }

    return {min_exposure, max_exposure};
}

void Keela::CameraManager::set_gain(double gain) {
    spdlog::info("Setting camera gain to {}", gain);

    ArvCamera *aravis_camera = get_aravis_camera();

    GError *error = nullptr;
    arv_camera_set_gain(aravis_camera, gain, &error);

    if (error != nullptr) {
        spdlog::error("Error setting gain on camera: {}", error->message);
        g_error_free(error);
        return;
    }

    // Read back the actual gain value to confirm it was set
    gdouble actual_gain = arv_camera_get_gain(aravis_camera, &error);

    if (error != nullptr) {
        spdlog::error("Error getting gain from camera: {}", error->message);
        g_error_free(error);
        return;
    }

    spdlog::info("Set gain to {:.1f} dB, actual camera gain: {:.1f} dB",
                 gain, actual_gain);
}

void Keela::CameraManager::set_exposure_time(double exposure) {
    spdlog::info("Setting camera exposure time to {}", exposure);

    ArvCamera *aravis_camera = get_aravis_camera();

    GError *error = nullptr;
    arv_camera_set_exposure_time(aravis_camera, exposure, &error);

    if (error != nullptr) {
        spdlog::error("Error setting exposure time on camera: {}", error->message);
        g_error_free(error);
        return;
    }
    // Read back the actual exposure time value to confirm it was set
    gdouble actual_exposure = arv_camera_get_exposure_time(aravis_camera, &error);
    if (error != nullptr) {
        spdlog::error("Error getting exposure time from camera: {}", error->message);
        g_error_free(error);
        return;
    }
    spdlog::info("Set exposure time to {:.1f} us, actual camera exposure time: {:.1f} us",
                 exposure, actual_exposure);
}

ArvCamera *Keela::CameraManager::get_aravis_camera() const {
    if (aravis_camera != nullptr) {
        return aravis_camera;
    }

    GstElement *camera_element = static_cast<GstElement *>(camera);
    // Try to get the underlying ArvCamera object from aravissrc
    g_object_get(camera_element, "camera", &aravis_camera, nullptr);
    return aravis_camera;
}

void Keela::CameraManager::start_recording() {
    std::string suffix = split_streams ? "even" : "";

    camera_stream_even->start_recording(get_filename(experiment_directory, this->id, suffix));

    if (split_streams) {
        camera_stream_odd->start_recording(get_filename(experiment_directory, this->id, "odd"));
    }
}

void Keela::CameraManager::stop_recording() {
    camera_stream_even->stop_recording();

    if (split_streams) {
        camera_stream_odd->stop_recording();
    }
}

GstPadProbeReturn Keela::CameraManager::frame_parity_probe_cb(GstPad *pad, GstPadProbeInfo *info, gpointer user_data) {
    GstBuffer *buffer = GST_PAD_PROBE_INFO_BUFFER(info);
    FrameProbeData *probe_data = static_cast<FrameProbeData *>(user_data);
    int parity = probe_data->parity;

    int frame_number = 0;

    // Some sources will have the frame number in the buffer offset (e.g. videotestsrc)
    if (GST_BUFFER_OFFSET(buffer) != GST_BUFFER_OFFSET_NONE) {
        frame_number = GST_BUFFER_OFFSET(buffer);
    }
    // But others like aravissrc do not set offset, so we fall back to our own per-camera counter
    else {
        frame_number = (*probe_data->counter)++;
    }

    if (frame_number % 2 == parity) {
        return GST_PAD_PROBE_OK; // Pass the frame
    } else {
        return GST_PAD_PROBE_DROP; // Drop the frame
    }
}

void Keela::CameraManager::set_frame_splitting(bool enabled) {
    split_streams = enabled;
    spdlog::info("Frame splitting {}", enabled ? "enabled" : "disabled");

    if (enabled) {
        if (camera_stream_odd == nullptr) {
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
        camera_stream_odd->Eject(false); // false = don't send EOS
        camera_stream_odd = nullptr; // clear the shared_ptr to allow re-creation later
        spdlog::info("Ejected camera_stream_odd from pipeline");
    }
}

void Keela::CameraManager::install_frame_splitting_probes() {
    spdlog::info("Installing frame splitting probes");

    // Install filtering probes on the sink pads of the even and odd tees
    GstPad *even_sink_pad = gst_element_get_static_pad(camera_stream_even->internal_tee, "sink");
    GstPad *odd_sink_pad = gst_element_get_static_pad(camera_stream_odd->internal_tee, "sink");

    if (even_sink_pad) {
        even_frame_probe_id = gst_pad_add_probe(even_sink_pad, GST_PAD_PROBE_TYPE_BUFFER,
                                                frame_parity_probe_cb, &even_probe_data, nullptr);
        g_object_unref(even_sink_pad);
        spdlog::info("Installed even frame filter probe");
    }

    if (odd_sink_pad) {
        odd_frame_probe_id = gst_pad_add_probe(odd_sink_pad, GST_PAD_PROBE_TYPE_BUFFER,
                                               frame_parity_probe_cb, &odd_probe_data, nullptr);
        g_object_unref(odd_sink_pad);
        spdlog::info("Installed odd frame filter probe");
    }
}

std::string Keela::CameraManager::get_filename(std::string directory, guint cam_id, std::string suffix) {
    time_t timestamp = std::time(nullptr);
    struct tm datetime = *localtime(&timestamp);
    std::stringstream ss;
    ss << std::put_time(&datetime, "%Y%m%d_%H%M%S_");

    if (suffix != "") {
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
    if (!sync_result) {
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
    if (even_frame_probe_id != 0) {
        GstPad *even_sink_pad = gst_element_get_static_pad(camera_stream_even->internal_tee, "sink");
        remove_probe_by_id(even_frame_probe_id, even_sink_pad, "even frame filter");
    }
    if (odd_frame_probe_id != 0) {
        GstPad *odd_sink_pad = gst_element_get_static_pad(camera_stream_odd->internal_tee, "sink");
        remove_probe_by_id(odd_frame_probe_id, odd_sink_pad, "odd frame filter");
    }
}
