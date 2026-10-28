//
// Helper functions for using custom Keela plugins
//

#include "keela-widgets/plugin_utils.h"

#include <aravis-0.8/arv.h>
#include <spdlog/spdlog.h>

namespace Keela {

bool is_custom_videotestsrc_available() {
	GstElementFactory* factory = gst_element_factory_find("keelavideotestsrc");
	if(factory) {
		gst_object_unref(factory);
		return true;
	}
	return false;
}

bool is_aravissrc_plugin_available() {
	GstElementFactory* factory = gst_element_factory_find("aravissrc");
	if(factory) {
		gst_object_unref(factory);
		return true;
	}
	return false;
}

bool is_aravis_camera_available() {
	// then check to see if any cameras are connected
	arv_update_device_list();
	unsigned int n_devices = arv_get_n_devices();

	if(n_devices > 0) {
		spdlog::info("Aravis cameras detected: {}", n_devices);
		// list all camera ids:
		for(unsigned int i = 0; i < n_devices; i++) {
			spdlog::info("  Camera {}: {}", i, arv_get_device_id(i));
		}
		return true;
	} else {
		spdlog::debug("No Aravis cameras detected");
		return false;
	}
}

std::string get_video_source_name() {
	if(aravis_camera_connected()) {
		spdlog::info("Using aravissrc plugin");
		return "aravissrc";
	}
#ifdef KEELA_VIDEOTESTSRC_ENABLED
	if(is_custom_videotestsrc_available()) {
		spdlog::info("Using custom keelavideotestsrc plugin");
		return "keelavideotestsrc";
	} else {
		spdlog::info("Custom videotestsrc enabled but not found - using standard videotestsrc plugin");
		return "videotestsrc";
	}
#else
	spdlog::info("Custom videotestsrc disabled at build time - using standard videotestsrc plugin");
	return "videotestsrc";
#endif
}

bool aravis_camera_connected() {
	spdlog::info("=== Camera Detection Status ===");

	bool plugin_available = is_aravissrc_plugin_available();
	bool camera_available = is_aravis_camera_available();

	spdlog::info("Aravis plugin available: {}", plugin_available ? "YES" : "NO");
	spdlog::info("Aravis camera connected: {}", camera_available ? "YES" : "NO");

	if(plugin_available && !camera_available) {
		spdlog::warn("Aravis plugin found but no camera detected - check camera connection");
		return false;
	} else if(!plugin_available) {
		spdlog::warn("Aravis plugin not found - check if aravis is installed");
		return false;
	}

	spdlog::info("================================");
	return true;
}
}  // namespace Keela
