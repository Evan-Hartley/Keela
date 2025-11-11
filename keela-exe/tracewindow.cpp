//
// Created by brand on 6/30/2025.
//

#include "tracewindow.h"

Keela::TraceWindow::TraceWindow() {
    signal_delete_event().connect(sigc::mem_fun(this, &TraceWindow::on_delete_event));

    set_default_size(640, 480);
    set_title("Traces");
    Window::add(scrolled_window);
    scrolled_window.add(container);
    Glib::signal_timeout().connect(sigc::mem_fun(this, &TraceWindow::on_timeout), 1000 / 60);
}

Keela::TraceWindow::~TraceWindow() {
}

void Keela::TraceWindow::addTraces(const std::vector<std::shared_ptr<Keela::ITraceable>>& traces_to_add) {
    if(traces_to_add.empty()) {
        return;
    }

    // Create a horizontal box to hold traces from this camera
    auto row_box = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL);
    row_box->set_spacing(10);

    for(const auto& trace : traces_to_add) {
        auto widget = std::make_shared<GLTraceRender>(trace);
        traces.push_back(widget);
        row_box->pack_start(*widget, true, true, 0);
    }

    container.pack_start(*row_box, false, false, 10);
    show_all_children();
    spdlog::info("TraceWindow::{}: Added {} traces in a row", __func__, traces_to_add.size());
}

void Keela::TraceWindow::removeTraceRow() {
    // Get the most recently added row box
    auto children = container.get_children();
    if(!children.empty()) {
        auto* last_row = children.back();
        container.remove(*last_row);

        // Count how many traces were in that last row
        auto last_row_box = dynamic_cast<Gtk::Box*>(last_row);
        int traces_in_last_row = last_row_box->get_children().size();

        // Remove the corresponding number of traces
        for(int i = 0; i < traces_in_last_row; i++) {
            traces.pop_back();
        }

        spdlog::info("TraceWindow::{}: Removed last trace row with {} traces", __func__, traces_in_last_row);
    }
}

int Keela::TraceWindow::num_traces() const {
    return traces.size();
}

void Keela::TraceWindow::set_on_closed_callback(std::function<void()> callback) {
    on_window_closed_callback = std::move(callback);
}

bool Keela::TraceWindow::on_timeout() {
    for(const auto& trace : traces) {
        trace->queue_draw();
    }
    return true;
}

bool Keela::TraceWindow::on_delete_event(GdkEventAny* any_event) {
    if(on_window_closed_callback) {
        on_window_closed_callback();
    }
    return false;  // Allow the default handler to destroy the window
}
