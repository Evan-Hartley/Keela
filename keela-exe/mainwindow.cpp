//
// Created by brand on 5/26/2025.
//

#include "mainwindow.h"

#include <keela-widgets/labeledspinbutton.h>
#include <keela-widgets/plugin_utils.h>
#include <spdlog/spdlog.h>

#include "cameracontrolwindow.h"
#include "keela-pipeline/consts.h"
#include "keela-widgets/framebox.h"

MainWindow::MainWindow() : Gtk::Window() {
	set_title("Main Control Window");
	set_resizable(true);
	set_default_size(800, 600);
	container.set_orientation(Gtk::ORIENTATION_VERTICAL);
	container.set_spacing(10);
	container.set_homogeneous(true);
	container.set_border_width(10);
	MainWindow::add(container);

	// Experiment Directory button
	directory_button.set_label("Select Experiment Directory");
	directory_button.signal_clicked().connect(sigc::mem_fun(this, &MainWindow::on_directory_clicked));
	container.add(directory_button);
	// Record button
	record_button.set_label("Start Recording");
	record_button.signal_clicked().connect(sigc::mem_fun(this, &MainWindow::on_record_button_clicked));
	Gdk::RGBA red;
	red.set_red(1.0);
	red.set_alpha(1.0);
	record_button.override_color(red);
	record_button.set_sensitive(false);
	record_button.set_tooltip_text("Experiment Directory must be selected in order to begin recording");
	container.add(record_button);

	pix_fmt_combo.m_combo.append(GRAY8, "8 Bit");
	pix_fmt_combo.m_combo.append(GRAY16_LE, "16 Bit (Little Endian)");
	pix_fmt_combo.m_combo.append(GRAY16_BE, "16 Bit (Big Endian)");
	pix_fmt_combo.m_combo.set_active_id(GRAY8);
	set_pix_fmt();
	container.add(pix_fmt_combo);
	// Framerate controls
	framerate_spin.m_spin.set_adjustment(Gtk::Adjustment::create(500, 1, 1000, 0.1));
	framerate_spin.m_spin.set_digits(1);
	container.add(framerate_spin);

	// calcium voltage recording setting control, controls split frame logic
	cv_recording_check.set_label("Calcium-Voltage Recording Setting");
	cv_recording_check.signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::on_split_frames_changed));
	cv_recording_check.set_active(should_split_frames);
	container.add(cv_recording_check);

	const auto camera_limits = Gtk::Adjustment::create(1.0, 1.0, std::numeric_limits<double>::max());
	num_camera_spin.m_spin.set_adjustment(camera_limits);
	num_camera_spin.m_spin.signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::on_camera_spin_changed));
	container.add(num_camera_spin);

	show_trace_check.set_label("Show Traces");
	trace_fps_spin.m_spin.set_adjustment(Gtk::Adjustment::create(DEFAULT_TRACE_FPS, 1, 1000, 1));
	trace_fps_spin.m_spin.signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::on_trace_fps_changed));

	trace_buffer_seconds_spin.m_spin.set_adjustment(Gtk::Adjustment::create(10, 1, 100, 1));
	trace_buffer_seconds_spin.m_spin.signal_value_changed().connect(
	    sigc::mem_fun(this, &MainWindow::on_trace_buffer_seconds_changed));

	trace_clear_buffer_button.signal_clicked().connect(
	    sigc::mem_fun(this, &MainWindow::on_trace_clear_buffer_button_clicked));

	// These components are only relevant when traces are enabled
	trace_control_row_box.set_sensitive(false);

	// Horizontal box for trace modifiers
	trace_control_row_box.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
	trace_control_row_box.set_spacing(10);
	trace_control_row_box.pack_start(trace_fps_spin);
	trace_control_row_box.pack_start(trace_buffer_seconds_spin);
	trace_control_row_box.pack_start(trace_clear_buffer_button);

	container.add(show_trace_check);
	container.pack_start(trace_control_row_box, false, false, 0);

	restart_camera_button.signal_clicked().connect(sigc::mem_fun(this, &MainWindow::reset_cameras));
	restart_camera_button.set_label("Restart Camera(s)");
	container.add(restart_camera_button);

	// Store connection so we can block it when closing the window programmatically
	trace_signal_connection =
	    show_trace_check.signal_clicked().connect(sigc::mem_fun(this, &MainWindow::on_trace_button_clicked));
	// create the pipeline
	pipeline = GST_PIPELINE(gst_pipeline_new("pipeline"));
	if(!pipeline) {
		throw std::runtime_error("Failed to create pipeline");
	}

	container.add(dump_graph_button);
	dump_graph_button.signal_clicked().connect(sigc::mem_fun(this, &MainWindow::dump_graph));
	// Initialize recording settings here
	on_camera_spin_changed();
	gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);

	// Query hardware capabilities to set ranges in the UI
	for(const auto &camera : cameras) {
		// Must be called after pipeline is in PLAYING state.
		// GStreamer elements are lazy and don't initialize hardware connections until pipeline starts playing.
		// We need access to the aravis camera to query supported ranges.
		int aravis_init_result = camera->init_aravis_controller();

		if(aravis_init_result != 0) {
			continue;
		}

		// Query the hardware capabilities to set ranges/options in the UI.
		// Also updates UI with the current hardware setting as reported by the camera.
		camera->update_binning_range();
		camera->update_gain_range();
		camera->update_exposure_time_range();
		camera->update_binning_modes();
	}

	show_all_children();
}

MainWindow::~MainWindow() {
	g_object_unref(pipeline);
}

void MainWindow::on_camera_spin_changed() {
	const auto curr = cameras.size();
	const auto next = static_cast<unsigned int>(num_camera_spin.m_spin.get_value_as_int());
	auto ret = gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_NULL);
	if(ret == GST_STATE_CHANGE_FAILURE) {
		spdlog::error("Failed to set state of pipeline");
	}
	if(next > curr) {
		for(guint i = curr; i < next; i++) {
			auto camera_id = i + 1;
			auto c = std::make_shared<Keela::CameraControlWindow>(camera_id, pix_fmt, should_split_frames);
			set_framerate(c->camera_manager.get());

			// Apply current trace framerate to new camera
			const auto fps = static_cast<guint>(trace_fps_spin.m_spin.get_value());
			c->set_trace_bin_framerate_caps(fps);

			g_object_ref(static_cast<GstElement *>(*c->camera_manager));
			auto inner_ret = gst_bin_add(GST_BIN(pipeline), *c->camera_manager);
			if(!inner_ret) {
				std::stringstream ss = std::stringstream();
				ss << "Failed to add camera " << std::to_string(camera_id) << " to pipeline";
				throw std::runtime_error(ss.str());
			}
			set_experiment_directory(c);
			cameras.push_back(c);
			if(trace_window != nullptr) {
				auto traces = c->get_traces();
				trace_window->addTraces(traces);
			}
		}
	} else if(next < curr) {
		for(guint i = curr; i > next; i--) {
			auto camera_win = std::move(cameras.back());
			gst_bin_remove(GST_BIN(pipeline), *camera_win->camera_manager);
			cameras.pop_back();
			if(trace_window != nullptr) {
				trace_window->removeTraceRow();
			}
		}
	}
	ret = gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);
	if(ret == GST_STATE_CHANGE_FAILURE) {
		spdlog::error("Failed to set state of pipeline");
	}
	dump_graph();
}

void MainWindow::on_record_button_clicked() {
	is_recording = !is_recording;
	auto button_text = is_recording ? "Recording" : "Start Recording";
	record_button.set_label(button_text);
	// set_sensitive
	framerate_spin.set_sensitive(!is_recording);
	cv_recording_check.set_sensitive(!is_recording);
	num_camera_spin.set_sensitive(!is_recording);
	show_trace_check.set_sensitive(!is_recording);
	restart_camera_button.set_sensitive(!is_recording);
	if(is_recording) {
		if(experiment_directory == "") {
			throw std::runtime_error("Experiment directory not specified");
		}
		directory_button.set_sensitive(false);
		set_state(GST_STATE_NULL);
		for(const auto &camera : cameras) {
			camera->camera_manager->start_recording();
		}
		// this resets the pipeline clock
		set_state(GST_STATE_PLAYING);
	} else {
		auto message_dialog = Gtk::MessageDialog("Remember to take calibration photos");
		message_dialog.run();
		for(const auto &camera : cameras) {
			camera->camera_manager->stop_recording();
		}
		directory_button.set_sensitive(true);
	}
}

void MainWindow::reset_cameras() {
	set_state(GST_STATE_NULL);
	// apply settings while the pipeline isn't actively playing
	set_framerate();
	set_pix_fmt();
	set_state(GST_STATE_PLAYING, false);
}

void MainWindow::set_state(GstState state, bool wait) {
	auto ret = gst_element_set_state(GST_ELEMENT(pipeline), state);
	switch(ret) {
		case GST_STATE_CHANGE_FAILURE:
			throw std::runtime_error("Failed to set state of pipeline to playing");
		case GST_STATE_CHANGE_ASYNC:
			ret = gst_element_get_state(GST_ELEMENT(pipeline), nullptr, nullptr, GST_CLOCK_TIME_NONE);
			if(ret == GST_STATE_CHANGE_FAILURE && wait) {
				throw std::runtime_error("Async state change failed");
			}
			break;
		default:
			spdlog::info("State change successful");
	}
}

void MainWindow::set_framerate() {
	spdlog::info("Updating framerate of all cameras");
	for(const auto &c : cameras) {
		set_framerate(c->camera_manager.get());
	}
}

void MainWindow::set_framerate(Keela::CameraManager *cm) const {
	const auto fr = framerate_spin.m_spin.get_value();
	cm->set_framerate(fr);
}

void MainWindow::set_pix_fmt() {
	spdlog::info("Updating pixel format of all cameras");
	this->pix_fmt = pix_fmt_combo.m_combo.get_active_id().raw();
	// inform all cameras of the pixel format change
	for(const auto &c : cameras) {
		c->set_pix_fmt(pix_fmt);
	}
}

void MainWindow::on_trace_button_clicked() {
	spdlog::info(__func__);
	if(trace_window == nullptr) {
		trace_window = std::make_unique<Keela::TraceWindow>();
		trace_window->set_on_closed_callback([this]() {
			spdlog::info("Trace window closed manually, performing cleanup");
			trace_window = nullptr;
			trace_control_row_box.set_sensitive(false);
			// Block the signal to prevent recursive calls
			trace_signal_connection.block();
			show_trace_check.set_active(false);
			trace_signal_connection.unblock();
		});
		trace_window->show();

		spdlog::debug("num traces: {}\t num cameras: {}", trace_window->num_traces(), trace_window->num_traces());
		for(unsigned int i = trace_window->num_traces(); i < cameras.size(); i++) {
			auto camera = cameras.at(i);
			auto traces = camera->get_traces();
			trace_window->addTraces(traces);
		}
		trace_control_row_box.set_sensitive(true);
	} else {
		trace_window = nullptr;
		trace_control_row_box.set_sensitive(false);
	}
}

void MainWindow::on_trace_fps_changed() {
	const auto fps = static_cast<guint>(trace_fps_spin.m_spin.get_value());
	spdlog::info("Setting trace framerate to {} fps for all cameras", fps);

	// Update trace framerate for all cameras
	for(const auto &camera : cameras) {
		camera->set_trace_bin_framerate_caps(fps);
	}
	// Update the trace plot duration based on the new framerate
	trace_window->set_trace_render_framerate(fps);
}

void MainWindow::on_trace_buffer_seconds_changed() {
	const auto trace_buffer_seconds = static_cast<guint>(trace_buffer_seconds_spin.m_spin.get_value());
	spdlog::info("Setting trace buffer retention to {} seconds for all cameras", trace_buffer_seconds);
	trace_window->set_trace_window_retention(trace_buffer_seconds);
}

void MainWindow::on_trace_clear_buffer_button_clicked() {
	spdlog::info("Clear trace buffer button clicked");
	trace_window->clear_trace_buffer();
}

void MainWindow::dump_graph() const {
	spdlog::info("{}: dumping pipeline graph", __func__);
	gst_debug_bin_to_dot_file(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "keelapipeline");
}

void MainWindow::on_directory_clicked() {
	if(is_recording) {
		throw std::runtime_error("Experiment directory already set");
	}

	Gtk::FileChooserDialog dialog =
	    Gtk::FileChooserDialog(*this, "Choose experiment directory", Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);

	dialog.add_button("Select", Gtk::RESPONSE_OK);
	dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);
	auto result = dialog.run();
	if(result == Gtk::RESPONSE_OK) {
		experiment_directory = dialog.get_filename();
		for(const auto &c : cameras) {
			set_experiment_directory(c);
		}
		record_button.set_tooltip_text("Current experiment directory: " + experiment_directory);
		record_button.set_sensitive(true);
	}
}

void MainWindow::set_experiment_directory(std::shared_ptr<Keela::CameraControlWindow> c) const {
	c->camera_manager->set_experiment_directory(experiment_directory);
}

void MainWindow::on_split_frames_changed() {
	should_split_frames = cv_recording_check.get_active();
	spdlog::info("Frame splitting set to {}", should_split_frames);

	for(const auto &c : cameras) {
		c->update_split_frame_state(should_split_frames);
	}

	// Update trace window if it's open
	if(trace_window != nullptr) {
		// Clear existing traces and re-add them based on new split frame state
		while(trace_window->num_traces() > 0) {
			trace_window->removeTraceRow();
		}

		// Add all traces for all cameras, with the correct split state
		for(const auto &camera : cameras) {
			auto traces = camera->get_traces();
			trace_window->addTraces(traces);
		}

		trace_control_row_box.set_sensitive(true);
	}
}
