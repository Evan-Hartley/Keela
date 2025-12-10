#include "keela-widgets/AravisController.h"

#include <spdlog/spdlog.h>

#include <string>
#include <vector>

namespace Keela {

AravisController::AravisController(GstElement *camera) : aravis_source(camera) {
	spdlog::info("AravisController initialized with camera pointer: {}", fmt::ptr(camera));

	g_object_get(aravis_source, "camera", &aravis_camera, nullptr);
	spdlog::info("Setting AravisController camera pointer to: {}", fmt::ptr(aravis_camera));
}

std::pair<double, double> AravisController::get_gain_range() const {
	if(aravis_camera == nullptr) {
		spdlog::warn("Gain control not supported");

		auto nan = std::numeric_limits<double>::quiet_NaN();
		return {nan, nan};
	}

	spdlog::debug("Querying gain range from camera hardware via ArvCamera object");

	double min_gain, max_gain;
	GError *error = nullptr;
	// Query the actual hardware gain limits
	arv_camera_get_gain_bounds(aravis_camera, &min_gain, &max_gain, &error);
	if(error == nullptr) {
		spdlog::info("Queried hardware gain range from camera: {:.1f} to {:.1f} dB", min_gain, max_gain);
	} else {
		spdlog::warn("Error querying gain range from camera: {}", error->message);
		g_error_free(error);
	}

	return {min_gain, max_gain};
}

double AravisController::get_gain() const {
	GError *error = nullptr;
	gdouble gain = arv_camera_get_gain(aravis_camera, &error);

	if(error != nullptr) {
		spdlog::error("Error getting gain from camera: {}", error->message);
		g_error_free(error);
		return std::numeric_limits<double>::quiet_NaN();
	}

	return gain;
}

std::pair<double, double> AravisController::get_exposure_time_range() const {
	if(aravis_camera == nullptr) {
		auto nan = std::numeric_limits<double>::quiet_NaN();
		return {nan, nan};
	}

	spdlog::debug("Querying exposure time range from camera hardware via ArvCamera object");

	double min_exposure, max_exposure;
	GError *error = nullptr;
	// Query the actual hardware exposure time limits
	arv_camera_get_exposure_time_bounds(aravis_camera, &min_exposure, &max_exposure, &error);
	if(error == nullptr) {
		spdlog::info("Queried hardware exposure time range from camera: {:.1f} to {:.1f} us", min_exposure,
		             max_exposure);
	} else {
		spdlog::warn("Error querying exposure time range from camera: {}", error->message);
		g_error_free(error);
	}

	return {min_exposure, max_exposure};
}

double AravisController::get_exposure_time() const {
	if(aravis_camera == nullptr) {
		spdlog::warn("No ArvCamera available - cannot get exposure time");
		return std::numeric_limits<double>::quiet_NaN();
	}

	spdlog::debug("Querying current exposure time from camera hardware via ArvCamera object");

	GError *error = nullptr;
	gint exposure_time = arv_camera_get_exposure_time(aravis_camera, &error);

	if(error != nullptr) {
		spdlog::error("Error getting exposure time from camera: {}", error->message);
		g_error_free(error);
		return std::numeric_limits<double>::quiet_NaN();
	}

	spdlog::info("Queried current camera exposure time: {} us", exposure_time);
	return exposure_time;
}

bool AravisController::supports_hardware_binning() const {
	return arv_camera_is_binning_available(aravis_camera, nullptr);
}

std::tuple<int, int, int, int> AravisController::get_binning_bounds() const {
	if(aravis_camera == nullptr) {
		spdlog::warn("Binning control not supported");
		auto nan = std::numeric_limits<int>::quiet_NaN();
		return {nan, nan, nan, nan};
	}

	spdlog::debug("Querying binning factor range from camera hardware via ArvCamera object");

	int min_x_binning, max_x_binning;
	int min_y_binning, max_y_binning;
	GError *error_x = nullptr;
	GError *error_y = nullptr;

	// Query the actual hardware binning limits
	arv_camera_get_x_binning_bounds(aravis_camera, &min_x_binning, &max_x_binning, &error_x);
	arv_camera_get_y_binning_bounds(aravis_camera, &min_y_binning, &max_y_binning, &error_y);

	if(error_x == nullptr && error_y == nullptr) {
		spdlog::info("Queried hardware binning factor range from camera: {} to {}", min_x_binning, max_x_binning);
	} else {
		spdlog::warn("Error querying binning factor range from camera: {} {}", error_x ? error_x->message : "no error",
		             error_y ? error_y->message : "no error");
		g_error_free(error_x);
		g_error_free(error_y);
	}

	return {min_x_binning, max_x_binning, min_y_binning, max_y_binning};
}

std::vector<std::string> AravisController::get_supported_binning_modes() {
	spdlog::info("Querying supported binning modes from camera hardware via ArvCamera object");
	std::vector<std::string> modes;
	if(aravis_camera == nullptr || !arv_camera_is_binning_available(aravis_camera, nullptr)) {
		spdlog::warn("Binning mode query not supported");
		return modes;
	}

	GError *error = nullptr;
	guint n_values = 0;
	auto mode_list =
	    arv_camera_dup_available_enumerations_as_strings(aravis_camera, "BinningHorizontalMode", &n_values, &error);
	if(error != nullptr) {
		spdlog::error("Error querying binning modes from camera: {}", error->message);
		g_error_free(error);
		g_free(mode_list);
		return modes;
	}

	if(mode_list == nullptr) {
		spdlog::warn("No binning modes returned from camera");
		g_free(mode_list);
		return modes;
	}

	for(guint i = 0; i < n_values; i++) {
		if(mode_list[i] != nullptr) {
			spdlog::debug("Found binning mode: {}", mode_list[i]);
			modes.emplace_back(mode_list[i]);
		}
	}

	g_free(mode_list);
	return modes;
}

std::pair<std::string, std::string> AravisController::get_binning_modes() const {
	auto empty = std::make_pair(std::string(""), std::string(""));
	if(aravis_camera == nullptr || !arv_camera_is_binning_available(aravis_camera, nullptr)) {
		spdlog::warn("Binning mode query not supported");
		return empty;
	}

	GError *h_error = nullptr;
	GError *v_error = nullptr;
	const char *h_mode = arv_camera_get_string(aravis_camera, "BinningHorizontalMode", &h_error);
	const char *v_mode = arv_camera_get_string(aravis_camera, "BinningVerticalMode", &v_error);

	if(h_error != nullptr || v_error != nullptr) {
		spdlog::warn("Error querying binning modes from camera: {} {}", h_error ? h_error->message : "no error",
		             v_error ? v_error->message : "no error");
		g_error_free(h_error);
		g_error_free(v_error);
		return empty;
	}

	spdlog::info("Queried current binning modes from camera: {} (H), {} (V)", h_mode, v_mode);
	return {std::string(h_mode), std::string(v_mode)};
}

std::pair<int, int> AravisController::get_binning_factors() const {
	auto nan = std::numeric_limits<int>::quiet_NaN();
	if(aravis_camera == nullptr || !arv_camera_is_binning_available(aravis_camera, nullptr)) {
		spdlog::warn("Binning factor query not supported");
		return {nan, nan};
	}

	spdlog::debug("Querying current binning factors from camera hardware via ArvCamera object");

	GError *error = nullptr;
	int x_binning, y_binning;
	arv_camera_get_binning(aravis_camera, &x_binning, &y_binning, &error);

	if(error != nullptr) {
		spdlog::warn("Error querying binning factors from camera: {}", error->message);
		g_error_free(error);
		return {nan, nan};
	}

	spdlog::info("Queried current binning factors from camera: {} (X), {} (Y)", x_binning, y_binning);
	return {x_binning, y_binning};
}

std::pair<int, int> AravisController::get_binning_increments() const {
	auto nan = std::numeric_limits<int>::quiet_NaN();
	if(aravis_camera == nullptr || !arv_camera_is_binning_available(aravis_camera, nullptr)) {
		spdlog::warn("Binning increment query not supported");
		return {nan, nan};
	}

	spdlog::debug("Querying binning factor increment from camera hardware via ArvCamera object");

	GError *error_x = nullptr;
	GError *error_y = nullptr;
	// Query the actual hardware binning increment steps
	int x_increment = arv_camera_get_x_binning_increment(aravis_camera, &error_x);
	int y_increment = arv_camera_get_y_binning_increment(aravis_camera, &error_y);

	if(error_x != nullptr || error_y != nullptr) {
		spdlog::warn("Error querying binning factor increment from camera: {} {}",
		             error_x ? error_x->message : "no error", error_y ? error_y->message : "no error");
		g_error_free(error_x);
		g_error_free(error_y);
		return {nan, nan};
	}

	spdlog::info("Queried hardware binning factor increment from camera: {} (X), {} (Y)", x_increment, y_increment);
	return {x_increment, y_increment};
}

std::pair<int, int> AravisController::get_current_resolution() const {
	auto nan = std::numeric_limits<int>::quiet_NaN();
	if(aravis_camera == nullptr) {
		spdlog::warn("No ArvCamera available - cannot get current resolution");
		return std::make_pair(nan, nan);
	}

	spdlog::debug("Querying current resolution from camera hardware via ArvCamera object");

	GError *w_error = nullptr;
	GError *h_error = nullptr;
	gint width = arv_camera_get_integer(aravis_camera, "Width", &w_error);
	gint height = arv_camera_get_integer(aravis_camera, "Height", &h_error);

	if(w_error != nullptr || h_error != nullptr) {
		spdlog::error("Error getting camera width: {}", w_error ? w_error->message : "no error");
		spdlog::error("Error getting camera height: {}", h_error ? h_error->message : "no error");
		g_error_free(w_error);
		g_error_free(h_error);

		return std::make_pair(nan, nan);
	}

	spdlog::info("Queried current camera resolution: {}×{}", width, height);
	return std::make_pair(width, height);
}

std::vector<std::string> AravisController::get_available_pixel_formats() const {
	// (unused, just logging and saving for when when we query the camera to set the combo_box dynamically)
	guint num_formats;
	auto pixel_formats = arv_camera_dup_available_pixel_formats_as_strings(aravis_camera, &num_formats, nullptr);

	std::vector<std::string> formats;

	spdlog::info("Available pixel formats:");
	for(guint i = 0; i < num_formats; ++i) {
		spdlog::info(" - {}", pixel_formats[i]);
		formats.emplace_back(pixel_formats[i]);
	}
	g_free(pixel_formats);

	return formats;
}

void AravisController::set_gain(double gain) {
	spdlog::info("Setting camera gain to {}", gain);
	g_object_set(aravis_source, "gain", gain, nullptr);
}

void AravisController::set_exposure_time(double exposure) {
	spdlog::info("Setting camera exposure time to {}", exposure);
	g_object_set(aravis_source, "exposure", exposure, nullptr);
}

void AravisController::set_binning_mode(const std::string &mode) {
	// @todo: this needs a utility function to not override the other settings in the features string
	//        currently this will override any other features every time
	spdlog::info("Setting camera binning mode to {}", mode);

	std::string bin_mode_feature = "BinningHorizontalMode=" + mode + " BinningVerticalMode=" + mode;
	g_object_set(aravis_source, "features", bin_mode_feature.c_str(), nullptr);

	spdlog::info("Set binning mode to {}", mode);
}

void AravisController::set_binning_factors(int binning_factor) {
	set_binning_factors(binning_factor, binning_factor);
}

void AravisController::set_binning_factors(int binning_factor_x, int binning_factor_y) {
	spdlog::info("Setting camera binning factors to x:{}, y:{}", binning_factor_x, binning_factor_y);

	g_object_set(aravis_source, "h-binning", binning_factor_x, "v-binning", binning_factor_y, nullptr);
}

}  // namespace Keela
