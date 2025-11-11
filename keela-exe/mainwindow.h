//
// Created by brand on 5/26/2025.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtkmm-3.0/gtkmm.h>
#include <keela-widgets/labeledspinbutton.h>

#include "cameracontrolwindow.h"
#include "tracewindow.h"

class MainWindow final : public Gtk::Window {
   public:
    MainWindow();

    ~MainWindow() override;

   private:
    GstPipeline *pipeline = nullptr;
    Gtk::Button record_button;
    Gtk::Button directory_button;
    Gtk::Button restart_camera_button;

    Keela::LabeledComboBoxText pix_fmt_combo = Keela::LabeledComboBoxText("Pixel Format");
    Keela::LabeledSpinButton framerate_spin = Keela::LabeledSpinButton("Framerate (Hz)");
    Keela::LabeledSpinButton data_matrix_w_spin = Keela::LabeledSpinButton("Data Width");
    Keela::LabeledSpinButton data_matrix_h_spin = Keela::LabeledSpinButton("Data Height");

    Gtk::CheckButton cv_recording_check;
    Keela::LabeledSpinButton num_camera_spin = Keela::LabeledSpinButton("Number of Cameras");

    Gtk::CheckButton show_trace_check;
    Gtk::Box trace_control_row_box;
    Keela::LabeledSpinButton trace_fps_spin = Keela::LabeledSpinButton("Trace Framerate (Hz)");
    Keela::LabeledSpinButton trace_buffer_seconds_spin = Keela::LabeledSpinButton("Trace Buffer Retention (seconds)");
    Gtk::Button trace_clear_buffer_button = Gtk::Button("Clear Trace Buffer");

    Gtk::Button dump_graph_button = Gtk::Button("Dump Pipeline Graph (debug)");

    Gtk::Box container;
    std::vector<std::shared_ptr<Keela::CameraControlWindow>> cameras;
    std::shared_ptr<Keela::TraceWindow> trace_window = nullptr;
    sigc::connection trace_signal_connection;

    void on_camera_spin_changed();

    void on_record_button_clicked();

    void reset_cameras();

    void set_state(GstState state, bool wait = true);

    void set_framerate();

    void set_framerate(Keela::CameraManager *cm) const;

    void set_resolution() const;

    void set_resolution(Keela::CameraControlWindow *c) const;

    void on_trace_button_clicked();

    void on_trace_fps_changed();

    void on_trace_buffer_seconds_changed();

    void on_trace_clear_buffer_button_clicked();

    void dump_graph() const;

    // TODO: maybe provide some platform dependent default?
    std::string experiment_directory = "";

    void on_directory_clicked();

    void set_experiment_directory(std::shared_ptr<Keela::CameraControlWindow> c) const;

    bool is_recording = false;

    bool should_split_frames = false;

    void on_split_frames_changed();

    void set_pix_fmt();
};

#endif  // MAINWINDOW_H
