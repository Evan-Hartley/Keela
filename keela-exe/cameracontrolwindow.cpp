//
// Created by brand on 5/30/2025.
//

#include "cameracontrolwindow.h"

#include <keela-widgets/framebox.h>
#include <spdlog/spdlog.h>

Keela::CameraControlWindow::CameraControlWindow(const guint id, std::string pix_fmt, bool should_split_frames) {
    this->id = id;
    spdlog::info("Creating {} for camera {}", __func__, id);
    camera_manager = std::make_unique<Keela::CameraManager>(id, pix_fmt, should_split_frames);

    set_title("Image control for Camera " + std::to_string(id));
    set_resizable(false);
    set_deletable(false);
    v_container.set_spacing(10);
    v_container.set_border_width(10);
    h_container.pack_start(v_container, false, false, 10);
    Window::add(h_container);
    const auto range_frame = Gtk::make_managed<Keela::FrameBox>("Range", Gtk::ORIENTATION_VERTICAL);
    range_check = Gtk::CheckButton("Range");
    // TODO: connect to changed signal on range check to control sensitivity value of range min/max spin
    range_check.signal_toggled().connect(sigc::mem_fun(*this, &CameraControlWindow::on_range_check_toggled));
    range_frame->add(range_check);

    range_min_spin.set_sensitive(false);
    range_max_spin.set_sensitive(false);
    range_min_spin.m_spin.set_adjustment(Gtk::Adjustment::create(0.0, 0.0, std::numeric_limits<uint8_t>::max()));
    range_max_spin.m_spin.set_adjustment(
        Gtk::Adjustment::create(std::numeric_limits<uint8_t>::max(), 0.0, std::numeric_limits<uint8_t>::max()));
    range_frame->add(range_min_spin);
    range_frame->add(range_max_spin);
    v_container.add(*range_frame);

    // Disable gain control until camera is ready and we can query for gain support and supported range
    gain_spin.m_spin.set_sensitive(false);
    gain_spin.m_spin.signal_value_changed().connect(sigc::mem_fun(*this, &CameraControlWindow::on_gain_changed));
    v_container.add(gain_spin);

    // Disable exposure time control until camera is ready and we can query for exposure time support and supported
    // range
    exposure_time_spin.m_spin.set_sensitive(false);
    exposure_time_spin.m_spin.signal_value_changed().connect(
        sigc::mem_fun(*this, &CameraControlWindow::on_exposure_time_changed));
    v_container.add(exposure_time_spin);

    // TODO: add rotation options
    rotation_combo.m_combo.signal_changed().connect(sigc::mem_fun(*this, &CameraControlWindow::on_rotation_changed));
    rotation_combo.m_combo.append(ROTATION_NONE, "---");
    rotation_combo.m_combo.append(ROTATION_90, "Rotate 90 degrees clockwise");
    rotation_combo.m_combo.append(ROTATION_180, "Rotate 180 degrees");
    rotation_combo.m_combo.append(ROTATION_270, "Rotate 270 degrees clockwise");
    rotation_combo.m_combo.set_active_id(ROTATION_NONE);

    v_container.add(rotation_combo);
    flip_horiz_check.signal_toggled().connect(sigc::mem_fun(*this, &CameraControlWindow::on_flip_horiz_changed));
    v_container.add(flip_horiz_check);
    flip_vert_check.signal_toggled().connect(sigc::mem_fun(*this, &CameraControlWindow::on_flip_vert_changed));
    v_container.add(flip_vert_check);

    // TODO: dynamically cast camera_manager->presentation to a WidgetElement to get a handle to a widget to add to the
    // window
    // TODO: do we need to take a snapshot of the odd stream too? maybe not because we just need it for scale
    // calibration?
    fetch_image_button.signal_clicked().connect(
        sigc::mem_fun(*camera_manager->camera_stream_even->snapshot, &Keela::SnapshotBin::take_snapshot));
    v_container.add(fetch_image_button);

    set_vexpand(false);

    // Create overlay for trace gizmo
    trace_gizmo_even = std::make_shared<TraceGizmo>();

    // Set up video presentations, frame_widget_even renders all frames unless split is enabled
    frame_widget_even = std::make_unique<VideoPresentation>("Camera " + std::to_string(id),
                                                            camera_manager->camera_stream_even->presentation,
                                                            640,  // width
                                                            480   // height
    );
    frame_widget_even->add_overlay_widget(*trace_gizmo_even);
    video_hbox.pack_start(*frame_widget_even, false, false, 10);

    if(camera_manager->is_frame_splitting_enabled()) {
        add_split_frame_ui();
    }

    h_container.pack_start(video_hbox, false, false, 10);

    auto gl_bin = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_VERTICAL);
    h_container.pack_start(*gl_bin, false, false, 10);

    show_all_children();
    show();
}

Keela::CameraControlWindow::~CameraControlWindow() {
    spdlog::info("Destroying camera control window {}", id);
}

void Keela::CameraControlWindow::on_range_check_toggled() {
    const auto active = range_check.get_active();
    range_min_spin.set_sensitive(active);
    range_max_spin.set_sensitive(active);
}

void Keela::CameraControlWindow::on_gain_changed() const {
    const auto gain = gain_spin.m_spin.get_value();
    spdlog::info("Gain changed to {}", gain);
    camera_manager->set_gain(gain);
}

void Keela::CameraControlWindow::on_exposure_time_changed() const {
    const auto exposure_time = exposure_time_spin.m_spin.get_value();
    spdlog::info("Exposure time changed to {}", exposure_time);
    camera_manager->set_exposure_time(exposure_time);
}

void Keela::CameraControlWindow::set_resolution(const int width, const int height) {
    spdlog::trace("resolution changed");
    this->camera_manager->set_resolution(width, height);
    // TODO: can we set a minimum size or allow the user to scale the gl area themselves?
    const auto rotation = rotation_combo.m_combo.get_active_id();
    if(rotation == ROTATION_90 || rotation == ROTATION_270) {
        frame_widget_even->set_video_size(height, width);
        if(frame_widget_odd) {
            frame_widget_odd->set_video_size(height, width);
        }
    } else {
        frame_widget_even->set_video_size(width, height);
        if(frame_widget_odd) {
            frame_widget_odd->set_video_size(width, height);
        }
    }
    m_width = width;
    m_height = height;
    // force the window to recalculate its size with respect to the size requests of its children
    resize(1, 1);
}

void Keela::CameraControlWindow::on_rotation_changed() {
    spdlog::trace("rotation changed");
    // NOTE: this appears to mess with the caps of the video stream
    const auto value = rotation_combo.m_combo.get_active_id();
    if(value == ROTATION_NONE) {
        camera_manager->transform.rotate_identity();
    } else if(value == ROTATION_90) {
        camera_manager->transform.rotate_90();
    } else if(value == ROTATION_180) {
        camera_manager->transform.rotate_180();
    } else if(value == ROTATION_270) {
        camera_manager->transform.rotate_270();
    } else {
        throw std::runtime_error(value + "is an invalid rotation");
    }
    if(m_width > 0 && m_height > 0) {
        set_resolution(m_width, m_height);
    } else {
        spdlog::warn("Skipping resolution change - resolution not initialized");
    }
}

void Keela::CameraControlWindow::on_flip_horiz_changed() const {
    const auto value = flip_horiz_check.get_active();
    camera_manager->transform.flip_horizontal(value);
}

void Keela::CameraControlWindow::on_flip_vert_changed() const {
    const auto value = flip_vert_check.get_active();
    camera_manager->transform.flip_vertical(value);
}

void Keela::CameraControlWindow::update_split_frame_state(bool should_split_frames) {
    camera_manager->set_frame_splitting(should_split_frames);
    if(should_split_frames) {
        spdlog::info("Adding split frame UI");
        add_split_frame_ui();
    } else {
        spdlog::info("Removing split frame UI");
        remove_split_frame_ui();
    }
    // Update traces whenever split frame state changes
    update_traces();
}

std::vector<std::shared_ptr<Keela::ITraceable>> Keela::CameraControlWindow::get_traces() {
    // Lazily create traces if not already done
    if(m_traces.empty()) {
        update_traces();
    }

    std::vector<std::shared_ptr<ITraceable>> traces;
    traces.reserve(m_traces.size());
    for(const auto& trace : m_traces) {
        traces.push_back(trace);
    }
    return traces;
}

void Keela::CameraControlWindow::update_traces() {
    m_traces.clear();

    // Always add the even trace
    std::string even_name = "Camera " + std::to_string(id);
    if(camera_manager->is_frame_splitting_enabled()) {
        even_name += " (Even)";
    }

    auto even_trace =
        std::make_shared<CameraTrace>(camera_manager->camera_stream_even->get_trace(), trace_gizmo_even, even_name);
    m_traces.push_back(even_trace);

    // Add odd trace if frame splitting is enabled
    if(camera_manager->is_frame_splitting_enabled() && trace_gizmo_odd) {
        auto odd_trace = std::make_shared<CameraTrace>(camera_manager->camera_stream_odd->get_trace(), trace_gizmo_odd,
                                                       "Camera " + std::to_string(id) + " (Odd)");
        m_traces.push_back(odd_trace);
    }
}

void Keela::CameraControlWindow::apply_trace_framerate(guint fps) {
    spdlog::info("Applying trace framerate {} to all traces for camera {}", fps, id);
    for(const auto& trace : m_traces) {
        auto trace_bin = trace->get_trace_bin();
        if(trace_bin) {
            trace_bin->set_trace_framerate(fps);
        }
    }
}

void Keela::CameraControlWindow::add_split_frame_ui() {
    if(frame_widget_odd)
        return;  // Already added

    // Create trace gizmo for odd frames
    trace_gizmo_odd = std::make_shared<TraceGizmo>();

    const auto rotation = rotation_combo.m_combo.get_active_id();
    if(rotation == ROTATION_90 || rotation == ROTATION_270) {
        frame_widget_odd =
            std::make_unique<VideoPresentation>("Odd Frames", camera_manager->camera_stream_odd->presentation,
                                                480,  // width
                                                640   // height
            );
    } else {
        frame_widget_odd =
            std::make_unique<VideoPresentation>("Odd Frames", camera_manager->camera_stream_odd->presentation,
                                                640,  // width
                                                480   // height
            );
    }

    frame_widget_odd->add_overlay_widget(*trace_gizmo_odd);
    video_hbox.pack_start(*frame_widget_odd, false, false, 10);

    // Ensure the new widget matches the current resolution
    if(m_width > 0 && m_height > 0) {
        const auto rotation = rotation_combo.m_combo.get_active_id();
        if(rotation == ROTATION_90 || rotation == ROTATION_270) {
            frame_widget_odd->set_video_size(m_height, m_width);
        } else {
            frame_widget_odd->set_video_size(m_width, m_height);
        }
    }

    show_all_children();
}

void Keela::CameraControlWindow::remove_split_frame_ui() {
    if(frame_widget_odd) {
        video_hbox.remove(*frame_widget_odd);
        frame_widget_odd.reset();
    }
}

void Keela::CameraControlWindow::update_gain_range() {
    // get the range supported by the camera hardware
    auto gain_range = camera_manager->get_gain_range();
    double min_gain = gain_range.first;
    double max_gain = gain_range.second;

    if(min_gain == 0.0 && max_gain == 0.0) {
        gain_spin.m_spin.set_sensitive(false);
        spdlog::warn("Gain control not supported by camera - disabling gain control UI");
        return;
    }
    // update the gain spin with the new range
    gain_spin.m_spin.set_adjustment(Gtk::Adjustment::create(min_gain, min_gain, max_gain, 1.0));
    gain_spin.m_spin.set_sensitive(true);

    spdlog::info("Updated gain control range to {:.1f} - {:.1f} dB", min_gain, max_gain);
}

void Keela::CameraControlWindow::update_exposure_time_range() {
    // get the range supported by the camera hardware
    auto exposure_time_range = camera_manager->get_exposure_time_range();
    double min_exposure_time = exposure_time_range.first;
    double max_exposure_time = exposure_time_range.second;

    if(min_exposure_time == 0.0 && max_exposure_time == 0.0) {
        exposure_time_spin.m_spin.set_sensitive(false);
        spdlog::warn("Exposure time control not supported by camera - disabling exposure time control UI");
        return;
    }
    // update the exposure time spin with the new range
    exposure_time_spin.m_spin.set_adjustment(
        Gtk::Adjustment::create(min_exposure_time, min_exposure_time, max_exposure_time, 1000.0));
    exposure_time_spin.m_spin.set_sensitive(true);

    spdlog::info("Updated exposure time control range to {:.1f} - {:.1f} μs", min_exposure_time, max_exposure_time);
}