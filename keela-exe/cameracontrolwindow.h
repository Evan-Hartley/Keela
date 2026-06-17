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
#include "keela-widgets/RangeControl.h"
#include "keela-widgets/tracegizmo.h"
#include "keela-widgets/videopresentation.h"

namespace Keela {
class CameraControlWindow final : public Gtk::Window,
                                  public IRestartable,
                                  public IRecordable {
   public:
	explicit CameraControlWindow(guint id, std::string pix_fmt, bool should_split_frames);

	~CameraControlWindow() override;

	std::shared_ptr<Keela::CameraManager> camera_manager;

	// void restart();

   private:
	Gtk::Box h_container = Gtk::Box();
	Gtk::Box v_container = Gtk::Box(Gtk::ORIENTATION_VERTICAL);
	Gtk::Box video_hbox = Gtk::Box(Gtk::ORIENTATION_HORIZONTAL);

	std::unique_ptr<VideoPresentation> video_presentation_even;
	std::unique_ptr<VideoPresentation> video_presentation_odd;

	Gtk::Button check_fr_button;
	Keela::LabeledSpinButton framerate_spin_real = Keela::LabeledSpinButton("Achieved Frame Rate (Hz)");
	sigc::connect fr_signal;
	//Gtk::CheckButton range_check = Gtk::CheckButton("Range");
	//Keela::LabeledSpinButton range_min_spin = Keela::LabeledSpinButton("Minimum");
	//Keela::LabeledSpinButton range_max_spin = Keela::LabeledSpinButton("Maximum");

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

	Keela::RangeControl range_control;

	guint id;

	double get_framerate();

	void update_framerate();

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

	std::vector<std::shared_ptr<CameraTrace>> m_traces;
};
}  // namespace Keela

#define ROTATION_NONE "No_Rotation"
#define ROTATION_90 "Rotation_90"
#define ROTATION_180 "Rotation_180"
#define ROTATION_270 "Rotation_270"

#endif  // CAMERACONTROLWINDOW_H
