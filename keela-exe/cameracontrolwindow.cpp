//
// Created by brand on 5/30/2025.
//

#include "cameracontrolwindow.h"

#include <keela-widgets/framebox.h>
#include <spdlog/spdlog.h>

#include "keela-pipeline/consts.h"

Keela::CameraControlWindow::CameraControlWindow(const guint id, std::string pix_fmt, bool should_split_frames)
    : camera_manager(std::make_shared<CameraManager>(id, should_split_frames)), pix_fmt_control(camera_manager) {
	this->id = id;
	spdlog::info("Creating {} for camera {}", __func__, id);

	// Frame Rate Listener
	check_fr_button.set_label("Check Frame Rate");
	check_fr_button.signal_clicked().connect(sigc::mem_fun(this, &CameraControlWindow::update_framerate));
	v_container.add(check_fr_button);
	double frame_rate_real = CameraControlWindow::get_framerate();
	framerate_spin_real.m_spin.set_adjustment(Gtk::Adjustment::create(frame_rate_real, frame_rate_real,frame_rate_real,1.0));
	framerate_spin_real.set_sensitive(false);
	framerate_spin_real.m_spin.set_digits(1);
	v_container.add(framerate_spin_real);


	// set_pix_fmt(pix_fmt);
	set_title("Image control for Camera " + std::to_string(id));
	set_resizable(false);
	set_deletable(false);
	v_container.set_spacing(10);
	v_container.set_border_width(10);
	h_container.pack_start(v_container, false, false, 10);
	Window::add(h_container);

	v_container.add(range_control);
	// pix_fmt_combo.init();
	v_container.add(pix_fmt_control);

	// Disable gain control until camera is ready and we can query for gain
	// support and supported range
	gain_spin.m_spin.set_sensitive(false);
	gain_signal =
	    gain_spin.m_spin.signal_value_changed().connect(sigc::mem_fun(*this, &CameraControlWindow::on_gain_changed));
	v_container.add(gain_spin);

	// Disable exposure time control until camera is ready and we can query for
	// exposure time support and supported range
	exposure_time_spin.m_spin.set_sensitive(false);
	exposure_time_signal = exposure_time_spin.m_spin.signal_value_changed().connect(
	    sigc::mem_fun(*this, &CameraControlWindow::on_exposure_time_changed));
	v_container.add(exposure_time_spin);

	/** Binning controls */
	bin_mode_combo.set_sensitive(false);
	bin_mode_signal = bin_mode_combo.m_combo.signal_changed().connect(
	    sigc::mem_fun(*this, &CameraControlWindow::on_bin_mode_changed));
	v_container.add(bin_mode_combo);

	bin_spin.m_spin.set_sensitive(false);
	bin_spin_signal =
	    bin_spin.m_spin.signal_value_changed().connect(sigc::mem_fun(*this, &CameraControlWindow::on_bin_spin_changed));
	v_container.add(bin_spin);

	rotation_combo.m_combo.append(ROTATION_NONE, "---");
	rotation_combo.m_combo.append(ROTATION_90, "Rotate 90 degrees clockwise");
	rotation_combo.m_combo.append(ROTATION_180, "Rotate 180 degrees");
	rotation_combo.m_combo.append(ROTATION_270, "Rotate 270 degrees clockwise");
	rotation_combo.m_combo.set_active_id(ROTATION_NONE);
	rotation_combo.m_combo.signal_changed().connect(sigc::mem_fun(*this, &CameraControlWindow::on_rotation_changed));

	v_container.add(rotation_combo);
	flip_horiz_check.signal_toggled().connect(sigc::mem_fun(*this, &CameraControlWindow::on_flip_horiz_changed));
	v_container.add(flip_horiz_check);
	flip_vert_check.signal_toggled().connect(sigc::mem_fun(*this, &CameraControlWindow::on_flip_vert_changed));
	v_container.add(flip_vert_check);

	// TODO: dynamically cast camera_manager->presentation to a WidgetElement to
	// get a handle to a widget to add to the window
	fetch_image_button.signal_clicked().connect(
	    sigc::mem_fun(*camera_manager->camera_stream_even->snapshot, &Keela::SnapshotBin::take_snapshot));
	v_container.add(fetch_image_button);

	set_vexpand(false);

	// Create overlay for trace gizmo
	trace_gizmo_even = std::make_shared<TraceGizmo>();

	// Set up video presentations, video_presentation_even renders all frames unless split is enabled
	video_presentation_even = std::make_unique<VideoPresentation>(
	    "Camera " + std::to_string(id), camera_manager->camera_stream_even->presentation, range_control);
	video_presentation_even->add_overlay_widget(*trace_gizmo_even);
	video_hbox.pack_start(*video_presentation_even, false, false, 10);

	if(camera_manager->is_frame_splitting_enabled()) {
		add_split_frame_ui();
	}

	h_container.pack_start(video_hbox, false, false, 10);

	auto gl_bin = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_VERTICAL);
	h_container.pack_start(*gl_bin, false, false, 10);

	// Set ranges and current values in UI based on camera capabilities
	if(camera_manager->has_aravis_controller()) {
		update_binning_range();
		update_gain_range();
		update_exposure_time_range();
		update_binning_modes();
	}

	show_all_children();
	show();

	pix_fmt_control.signal_pix_fmt_changed.connect(sigc::mem_fun(range_control,&RangeControl::on_pix_fmt_changed));
	// Chain our signal handlers together
	IRestartable::chain(*camera_manager);
	IRestartable::chain(pix_fmt_control);

	IRecordable::chain(*camera_manager);
	IRecordable::chain(pix_fmt_control);

	signal_restart.emit();
}

Keela::CameraControlWindow::~CameraControlWindow() {
	spdlog::info("Destroying camera control window {}", id);
}

double Keela::CameraControlWindow::get_framerate() {
	double fr_real = camera_manager->get_framerate();
	return fr_real;
}

void Keela::CameraControlWindow::update_framerate() {
	// Update the frame rate spin with the new range and current setting
	fr_signal.block();
	framerate_spin_real.set_sensitive(true);
	double frame_rate_real = CameraControlWindow::get_framerate();
	framerate_spin_real.m_spin.set_adjustment(Gtk::Adjustment::create(frame_rate_real,frame_rate_real,frame_rate_real,1.0));
	framerate_spin_real.set_sensitive(false);
	fr_signal.unblock();
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

void Keela::CameraControlWindow::on_bin_mode_changed() {
	const auto mode = bin_mode_combo.m_combo.get_active_id();
	spdlog::info("Binning mode changed to {}", mode.raw());
	camera_manager->set_binning_mode(mode);
}

void Keela::CameraControlWindow::on_bin_spin_changed() const {
	const auto binning_factor = static_cast<int>(bin_spin.m_spin.get_value());
	spdlog::info("Binning factor changed to {}", binning_factor);
	camera_manager->set_binning_factors(binning_factor);
}

void Keela::CameraControlWindow::on_rotation_changed() {
	spdlog::trace("rotation changed");
	const auto value = rotation_combo.m_combo.get_active_id();

	update_presentation_sizes(value);

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
}

void Keela::CameraControlWindow::update_presentation_sizes(const std::string &rotation) {
	if(rotation == ROTATION_90 || rotation == ROTATION_270) {
		video_presentation_even->swap_dimensions();
		if(video_presentation_odd) {
			video_presentation_odd->swap_dimensions();
		}
	} else {
		video_presentation_even->reset_dimensions();
		if(video_presentation_odd) {
			video_presentation_odd->reset_dimensions();
		}
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
	for(const auto &trace : m_traces) {
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

	auto even_trace = std::make_shared<CameraTrace>(camera_manager->camera_stream_even->get_trace_bin(),
	                                                trace_gizmo_even, *video_presentation_even, even_name);
	m_traces.push_back(even_trace);

	// Add odd trace if frame splitting is enabled
	if(camera_manager->is_frame_splitting_enabled() && trace_gizmo_odd) {
		auto odd_trace =
		    std::make_shared<CameraTrace>(camera_manager->camera_stream_odd->get_trace_bin(), trace_gizmo_odd,
		                                  *video_presentation_odd, "Camera " + std::to_string(id) + " (Odd)");
		m_traces.push_back(odd_trace);
	}
}

void Keela::CameraControlWindow::set_trace_bin_framerate_caps(guint fps) {
	spdlog::info("Applying trace framerate {} to all traces for camera {}", fps, id);
	for(const auto &trace : m_traces) {
		auto trace_bin = trace->get_trace_bin();
		if(trace_bin) {
			trace_bin->set_trace_framerate(fps);
		}
	}
}

void Keela::CameraControlWindow::add_split_frame_ui() {
	if(video_presentation_odd)
		return;  // Already added

	// Create trace gizmo for odd frames
	trace_gizmo_odd = std::make_shared<TraceGizmo>();

	const auto rotation = rotation_combo.m_combo.get_active_id();
	video_presentation_odd =
	    std::make_unique<VideoPresentation>("Odd Frames", camera_manager->camera_stream_odd->presentation, range_control,
	                                        DEFAULT_PRESENTATION_WIDTH, DEFAULT_PRESENTATION_HEIGHT);
	if(rotation == ROTATION_90 || rotation == ROTATION_270) {
		video_presentation_odd->swap_dimensions();
	}

	video_presentation_odd->add_overlay_widget(*trace_gizmo_odd);
	video_hbox.pack_start(*video_presentation_odd, false, false, 10);

	show_all_children();
}

void Keela::CameraControlWindow::remove_split_frame_ui() {
	if(video_presentation_odd) {
		video_hbox.remove(*video_presentation_odd);
		video_presentation_odd.reset();
	}
}

void Keela::CameraControlWindow::update_gain_range() {
	// get the range supported by the camera hardware
	auto gain_range = camera_manager->get_gain_range();
	double min_gain = gain_range.first;
	double max_gain = gain_range.second;
	// and the current gain to set the initial spin value
	auto current_gain = camera_manager->get_gain();

	if(std::isnan(min_gain)) {
		gain_spin.m_spin.set_sensitive(false);
		spdlog::warn("Gain control not supported by camera - disabling gain control UI");
		return;
	}
	// update the gain spin with the new range and current setting
	gain_signal.block();
	gain_spin.m_spin.set_adjustment(Gtk::Adjustment::create(min_gain, min_gain, max_gain, 1.0));
	gain_spin.m_spin.set_value(current_gain);
	gain_spin.m_spin.set_sensitive(true);
	gain_signal.unblock();

	spdlog::info("Updated gain control range to {:.1f} - {:.1f} dB", min_gain, max_gain);
}

void Keela::CameraControlWindow::update_exposure_time_range() {
	// get the range supported by the camera hardware
	auto [min_exposure_time, max_exposure_time] = camera_manager->get_exposure_time_range();
	// and the current exposure time to set the initial spin value
	auto current_exposure_time = camera_manager->get_exposure_time();

	if(std::isnan(min_exposure_time)) {
		exposure_time_spin.m_spin.set_sensitive(false);
		spdlog::warn("Exposure time control not supported by camera - disabling exposure time control UI");
		return;
	}
	// update ui based on hardware capabilities and current setting
	exposure_time_signal.block();
	exposure_time_spin.m_spin.set_adjustment(
	    Gtk::Adjustment::create(min_exposure_time, min_exposure_time, max_exposure_time, 1000.0));
	exposure_time_spin.m_spin.set_value(current_exposure_time);
	exposure_time_spin.m_spin.set_sensitive(true);
	exposure_time_signal.unblock();

	spdlog::info("Updated exposure time control range to {:.1f} - {:.1f} μs", min_exposure_time, max_exposure_time);
}

void Keela::CameraControlWindow::update_binning_modes() {
	auto modes = camera_manager->get_supported_binning_modes();
	auto current_mode = camera_manager->get_binning_modes();

	if(modes.empty()) {
		bin_mode_combo.m_combo.set_sensitive(false);
		spdlog::warn("Binning mode control not supported by camera - disabling binning mode UI");
		return;
	}
	spdlog::info("Updating binning modes UI with {} modes", modes.size());
	bin_mode_combo.set_sensitive(true);
	bin_mode_combo.m_combo.remove_all();
	for(const auto &mode : modes) {
		spdlog::info("Adding binning mode to UI: {}", mode);
		bin_mode_combo.m_combo.append(mode, mode);
	}
	bin_mode_signal.block();
	bin_mode_combo.m_combo.set_active_id(current_mode.first);
	bin_mode_signal.unblock();
}

void Keela::CameraControlWindow::update_binning_range() {
	bool supports_hardware_binning = camera_manager->supports_hardware_binning();

	if(!supports_hardware_binning) {
		spdlog::warn("Hardware binning control not supported by camera.");
		return;
	}
	// get the range supported by the camera hardware,
	// only using x as we don't support independent gain settings
	auto [min_x_binning, max_x_binning, min_y_binning, max_y_binning] = camera_manager->get_binning_bounds();
	auto [x_binning_increment, y_binning_increment] = camera_manager->get_binning_increments();

	// Get the current binning setting from hardware to preserve it
	auto [current_x_binning, current_y_binning] = camera_manager->get_binning_factors();
	double current_binning = std::isnan(current_x_binning) ? min_x_binning : current_x_binning;

	bin_spin_signal.block();
	// update the binning spin with the new range, preserving current value
	bin_spin.m_spin.set_adjustment(
	    Gtk::Adjustment::create(current_binning, min_x_binning, max_x_binning, x_binning_increment));

	bin_spin.m_spin.set_value(current_binning);
	bin_spin_signal.unblock();

	bin_spin.m_spin.set_sensitive(true);

	spdlog::info("Updated binning factor control range to {} - {}, current value: {}", min_x_binning, max_x_binning,
	             current_binning);
}
