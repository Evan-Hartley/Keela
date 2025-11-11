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
#include "keela-widgets/tracegizmo.h"
#include "keela-widgets/videopresentation.h"

namespace Keela {
class CameraControlWindow final : public Gtk::Window {
   public:
    explicit CameraControlWindow(guint id, std::string pix_fmt, bool should_split_frames);

    ~CameraControlWindow() override;

    std::shared_ptr<Keela::CameraManager> camera_manager;

    void set_resolution(int width, int height);

   private:
    Gtk::Box h_container = Gtk::Box();
    Gtk::Box v_container = Gtk::Box(Gtk::ORIENTATION_VERTICAL);
    Gtk::Box video_hbox = Gtk::Box(Gtk::ORIENTATION_HORIZONTAL);

    std::unique_ptr<VideoPresentation> frame_widget_even;
    std::unique_ptr<VideoPresentation> frame_widget_odd;

    Gtk::CheckButton range_check = Gtk::CheckButton("Range");
    Keela::LabeledSpinButton range_min_spin = Keela::LabeledSpinButton("Minimum");
    Keela::LabeledSpinButton range_max_spin = Keela::LabeledSpinButton("Maximum");

    // TODO: histogram
    Keela::LabeledSpinButton gain_spin = Keela::LabeledSpinButton("Gain (dB)");
    Keela::LabeledSpinButton exposure_time_spin = Keela::LabeledSpinButton("Exposure Time (μs)");

    Keela::LabeledComboBoxText rotation_combo = Keela::LabeledComboBoxText("Select Rotation");
    Gtk::CheckButton flip_horiz_check = Gtk::CheckButton("Flip Along Horizontal Center");
    Gtk::CheckButton flip_vert_check = Gtk::CheckButton("Flip Along Vertical Center");
    Gtk::Button fetch_image_button = Gtk::Button("Fetch Image");

    std::shared_ptr<Keela::TraceGizmo> trace_gizmo_even;
    std::shared_ptr<Keela::TraceGizmo> trace_gizmo_odd;

    guint id;

    void on_range_check_toggled();

    void on_gain_changed() const;

    void on_exposure_time_changed() const;

    void on_rotation_changed();

    void on_flip_horiz_changed() const;

    void on_flip_vert_changed() const;

    void add_split_frame_ui();

    void remove_split_frame_ui();

    void update_traces();

   public:
    std::vector<std::shared_ptr<ITraceable>> get_traces();
    void apply_trace_framerate(guint fps);

    // Method for main window to toggle split frame mode
    void update_split_frame_state(bool enabled);

    // Update gain range after camera is ready
    void update_gain_range();

    // Update exposure time range after camera is ready
    void update_exposure_time_range();

   private:
    std::vector<std::shared_ptr<CameraTrace>> m_traces;
    // set width and height initially to a sentinel value
    int m_width = -1, m_height = -1;
};
}  // namespace Keela

#define ROTATION_NONE "No_Rotation"
#define ROTATION_90 "Rotation_90"
#define ROTATION_180 "Rotation_180"
#define ROTATION_270 "Rotation_270"

#endif  // CAMERACONTROLWINDOW_H
