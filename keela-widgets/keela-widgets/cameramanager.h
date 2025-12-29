//
// Created by brand on 6/3/2025.
//

#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H
#include <aravis-0.8/arv.h>
#include <keela-pipeline/CameraStreamBin.h>
#include <keela-pipeline/bin.h>
#include <keela-pipeline/caps.h>
#include <keela-pipeline/presentationbin.h>
#include <keela-pipeline/recordbin.h>
#include <keela-pipeline/simpleelement.h>
#include <keela-pipeline/snapshotbin.h>
#include <keela-pipeline/splitstreambin.h>
#include <keela-pipeline/transformbin.h>
#include <keela-widgets/AravisController.h>

#include <atomic>
#include <set>

#define EVEN_FRAME 0  // TODO: obsolete
#define ODD_FRAME 1   // TODO: obsolete

namespace Keela {
class CameraManager final : public Keela::Bin {
   public:
	explicit CameraManager(guint id, bool split_streams);

	~CameraManager() override;
#pragma region Aravis Stuff
	bool has_aravis_controller() const {
		return aravis_controller != nullptr;
	}

	std::vector<std::string> get_available_pixel_formats() const;

	void set_pix_fmt(const std::string &format);

	void set_framerate(double framerate);

	void set_experiment_directory(const std::string &path);

	// Query hardware capabilities
	std::pair<double, double> get_gain_range() const;

	double get_gain() const;

	std::pair<double, double> get_exposure_time_range() const;

	double get_exposure_time() const;

	bool supports_hardware_binning() const;

	std::vector<std::string> get_supported_binning_modes() const;

	std::pair<std::string, std::string> get_binning_modes() const;

	std::tuple<int, int, int, int> get_binning_bounds() const;

	std::pair<int, int> get_binning_increments() const;

	/**
	 * Get the current camera resolution after any binning/cropping applied
	 *
	 * Returns {nan, nan} if camera is not available
	 */
	std::pair<int, int> get_current_resolution() const;

	/**
	 * Get the current binning factors
	 *
	 * Returns {nan, nan} if camera is not available
	 */
	std::pair<int, int> get_binning_factors() const;
	void set_gain(double gain);

	void set_exposure_time(double exposure);

	void set_binning_mode(std::string mode);

	void set_binning_factors(int binning_factor_both);
	void set_binning_factors(int binning_factor_x, int binning_factor_y);
#pragma endregion Aravis Stuff

	void start_recording();

	void stop_recording();

	SimpleElement camera;

#pragma region Frame Splitting Stuff
	// Control frame splitting
	void set_frame_splitting(bool split_enabled);

	bool is_frame_splitting_enabled() const {
		if(!stream) {
			throw std::logic_error("stream is null");
		}
		auto maybe_split_stream = std::dynamic_pointer_cast<SplitStreamBin>(stream);
		auto maybe_non_split_stream = std::dynamic_pointer_cast<CameraStreamBin>(stream);
		if(!(maybe_non_split_stream != nullptr || maybe_split_stream != nullptr)) {
			// this can only happen if stream is assigned to anything aside from CameraStreamBin or SplitStreamBin
			throw std::logic_error("stream is an unknown subtype");
		}
		return maybe_split_stream != nullptr && maybe_non_split_stream == nullptr;
	}

	std::vector<std::shared_ptr<Keela::CameraStreamBin>> get_streams() const;

   private:
	// TODO: common interface for start/stop recording + ejectable
	std::shared_ptr<Keela::EjectableElement> stream = nullptr;

#pragma endregion Frame Splitting Stuff
	guint id;
	bool split_streams;

	/// caps filter to apply to the entire stream
	Caps base_caps;

	/// for now, experiment directory will be set to my temp directory until I
	/// figure out gtk file dialogs
	std::string experiment_directory = "C:\\temp";

	/*
	 * prepends the filename with the current time to avoid overwriting files
	 * ex: directory/20250915_181211_cam_1.mkv
	 *     directory/20250915_181211_cam_1_even.mkv (if frame splitting is
	 * enabled) supports cross-platform path joining
	 */
	static std::string get_filename(std::string directory, guint cam_id, std::string suffix = "");

	void set_pipeline_state(GstState state);

	/**
	 * Stops and restarts the pipeline so we can apply changes to the camera/aravissrc.
	 */
	void restart_pipeline();

	/**
	 * Manages Aravis camera hardware settings like querying for
	 * hardware capabilities and adjusting camera parameters.
	 */
	std::unique_ptr<AravisController> aravis_controller = nullptr;

	SimpleElement caps_filter = SimpleElement("capsfilter");
	SimpleElement tee = SimpleElement("tee");

   public:
	TransformBin transform = TransformBin("transform");

	SnapshotBin snapshot = SnapshotBin("snapshot");
};
}  // namespace Keela
#endif  // CAMERAMANAGER_H
