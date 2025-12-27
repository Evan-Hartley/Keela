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
#include <keela-pipeline/transformbin.h>
#include <keela-widgets/AravisController.h>

#include <atomic>
#include <set>

#define EVEN_FRAME 0  // TODO: obsolete
#define ODD_FRAME 1   // TODO: obsolete

namespace Keela {
// Structure to pass both parity and counter to frame probe callback
struct FrameProbeData {
	int parity;
	guint64 *counter;
};
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
	void set_frame_splitting(bool enabled);

	bool is_frame_splitting_enabled() const {
		return split_streams;
	}

	// Camera Streams manage presentation, recording, and tracing of their
	// respective frame streams
	[[obsolete("obsoleted by SplitStreamBin")]]
	std::shared_ptr<CameraStreamBin> camera_stream_even = std::make_shared<CameraStreamBin>("camera_stream_even");
	[[obsolete("obsoleted by SplitStreamBin")]]
	std::shared_ptr<CameraStreamBin> camera_stream_odd = std::make_shared<CameraStreamBin>("camera_stream_odd");

	SimpleElement caps_filter = SimpleElement("capsfilter");
	TransformBin transform = TransformBin("transform");

   private:
	[[obsolete("obsoleted by SplitStreamBin")]]
	gulong even_frame_probe_id = 0;
	[[obsolete("obsoleted by SplitStreamBin")]]
	gulong odd_frame_probe_id = 0;

	// Per-camera frame counter for sources that don't set buffer offset
	[[obsolete("obsoleted by SplitStreamBin")]]
	guint64 manual_frame_counter = 0;

	// Data structures for frame probes
	[[obsolete("obsoleted by SplitStreamBin")]]
	FrameProbeData even_probe_data{EVEN_FRAME, &manual_frame_counter};
	[[obsolete("obsoleted by SplitStreamBin")]]
	FrameProbeData odd_probe_data{ODD_FRAME, &manual_frame_counter};

	//[[obsolete("obsoleted by SplitStreamBin")]]
	// void set_up_frame_splitting();

	[[obsolete("obsoleted by SplitStreamBin")]]
	void install_frame_splitting_probes();

	[[obsolete("obsoleted by SplitStreamBin")]]
	void remove_frame_splitting_probes();

	[[obsolete("obsoleted by SplitStreamBin")]]
	void remove_probe_by_id(gulong &probe_id, GstPad *pad, const std::string &probe_name);

	// Frame filtering callback
	[[obsolete("obsoleted by SplitStreamBin")]]
	static GstPadProbeReturn frame_parity_probe_cb(GstPad *pad, GstPadProbeInfo *info, gpointer user_data);

	void add_odd_camera_stream();

	/**
	 * use to split a stream into as many identical streams as we want.
	 *
	 * NOTE: any elements that come after "tee" should probably inherit from
	 * Keela::QueueBin
	 */
	[[obsolete("obsoleted by SplitStreamBin")]]
	SimpleElement tee_main = SimpleElement("tee");
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
};
}  // namespace Keela
#endif  // CAMERAMANAGER_H
