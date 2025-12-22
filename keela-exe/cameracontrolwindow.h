//
// Created by brand on 5/30/2025.
//

#ifndef CAMERACONTROLWINDOW_H
#define CAMERACONTROLWINDOW_H
#include <gtkmm-3.0/gtkmm.h>
#include <keela-widgets/labeledcomboboxtext.h>
#include <keela-widgets/labeledspinbutton.h>

#include "../keela-widgets/keela-widgets/cameramanager.h"
#include "../keela-widgets/keela-widgets/cameratrace.h"
#include "keela-widgets/GLCameraRender.h"
#include "keela-widgets/GLTraceRender.h"
#include "keela-widgets/PixFmtControl.h"
#include "keela-widgets/tracegizmo.h"
#include "keela-widgets/videopresentation.h"

namespace Keela {
class CameraControlWindow final : public Gtk::Window,
                                  public Keela::IControlGLCameraRenderHeatmap,
                                  public IRestartable,
                                  public IRecordable {
   public:
	explicit CameraControlWindow(guint id, std::string pix_fmt, bool should_split_frames);

	~CameraControlWindow() override;

	std::shared_ptr<Keela::CameraManager> camera_manager;

	[[obsolete]]
	void set_pix_fmt(std::string pix_fmt);

	// void restart();

   private:
	Gtk::Box h_container = Gtk::Box();
	Gtk::Box v_container = Gtk::Box(Gtk::ORIENTATION_VERTICAL);
	Gtk::Box video_hbox = Gtk::Box(Gtk::ORIENTATION_HORIZONTAL);

	std::unique_ptr<VideoPresentation> video_presentation_even;
	std::unique_ptr<VideoPresentation> video_presentation_odd;

	Gtk::CheckButton range_check = Gtk::CheckButton("Range");
	Keela::LabeledSpinButton range_min_spin = Keela::LabeledSpinButton("Minimum");
	Keela::LabeledSpinButton range_max_spin = Keela::LabeledSpinButton("Maximum");

	// TODO: histogram
	Keela::LabeledSpinButton gain_spin = Keela::LabeledSpinButton("Gain (dB)");
	sigc::connection gain_signal;
	Keela::LabeledSpinButton exposure_time_spin = Keela::LabeledSpinButton("Exposure Time (μs)");
	sigc::connection exposure_time_signal;

	Keela::LabeledSpinButton bin_spin = Keela::LabeledSpinButton("Binning Factor");
	sigc::connection bin_spin_signal;
	Keela::LabeledComboBoxText bin_mode_combo = Keela::LabeledComboBoxText("Binning Mode");
	sigc::connection bin_mode_signal;

	Keela::LabeledComboBoxText rotation_combo = Keela::LabeledComboBoxText("Select Rotation");
	Gtk::CheckButton flip_horiz_check = Gtk::CheckButton("Flip Along Horizontal Center");
	Gtk::CheckButton flip_vert_check = Gtk::CheckButton("Flip Along Vertical Center");
	Gtk::Button fetch_image_button = Gtk::Button("Fetch Image");

	std::shared_ptr<Keela::TraceGizmo> trace_gizmo_even;
	std::shared_ptr<Keela::TraceGizmo> trace_gizmo_odd;

	Keela::PixFmtControl pix_fmt_control;

	guint id;

	void on_range_check_toggled();

	void on_gain_changed() const;

	void on_exposure_time_changed() const;

	void on_bin_mode_changed();

	void on_bin_spin_changed() const;

	void on_rotation_changed();

	void on_flip_horiz_changed() const;

	void on_flip_vert_changed() const;

	void add_split_frame_ui();

	void remove_split_frame_ui();

	void update_traces();

	void update_presentation_sizes(const std::string &rotation);

   public:
	std::vector<std::shared_ptr<ITraceable>> get_traces();
	void set_trace_bin_framerate_caps(guint fps);

	// Method for main window to toggle split frame mode
	void update_split_frame_state(bool enabled);

	// Update gain range after camera is ready
	void update_gain_range();

	// Update exposure time range after camera is ready
	void update_exposure_time_range();

	void update_binning_modes();

	void update_binning_range();

   private:
	bool is_heatmap_enabled() override;

	float heatmap_min() override;

	float heatmap_max() override;
	std::vector<IRecordable> recordable_children() override {
		std::vector<IRecordable> children{static_cast<IRecordable>(*camera_manager),
		                                  static_cast<IRecordable>(pix_fmt_control)};
		return children;
	}

   protected:
	std::vector<IRestartable> restartable_children() override {
		std::vector<IRestartable> children{static_cast<IRestartable>(*camera_manager),
		                                   static_cast<IRestartable>(pix_fmt_control)};
		return children;
	}

   private:
	// scaling factor to apply to heatmap_max and heatmap_min to ensure they remain in the range [0,1]
	float heatmap_scale;

	std::vector<std::shared_ptr<CameraTrace>> m_traces;
};
}  // namespace Keela

#define ROTATION_NONE "No_Rotation"
#define ROTATION_90 "Rotation_90"
#define ROTATION_180 "Rotation_180"
#define ROTATION_270 "Rotation_270"

#endif  // CAMERACONTROLWINDOW_H
