//
// Created by brand on 6/30/2025.
//

#ifndef TRACEWINDOW_H
#define TRACEWINDOW_H
#include <gtkmm/box.h>
#include <gtkmm/window.h>

#include "keela-widgets/GLTraceRender.h"

namespace Keela {
class TraceWindow final : public Gtk::Window {
	// TODO: sync GLTraceRender::queue_draw across every instance of GLTraceRender to avoid jittering
   public:
	TraceWindow();

	~TraceWindow() override;

	void addTraces(const std::vector<std::shared_ptr<Keela::ITraceable>>& traces);

	/** A row contains all of the traces for one camera. */
	void removeTraceRow();

	int num_traces() const;

	/** Callback passed by the MainWindow to update "Show Traces" UI when we close the trace window */
	void set_on_closed_callback(std::function<void()> callback);

	void set_trace_window_retention(guint trace_retention_seconds);

	void set_trace_render_framerate(double framerate);

	void clear_trace_buffer();

   private:
	std::vector<std::shared_ptr<GLTraceRender>> traces;
	Gtk::ScrolledWindow scrolled_window;
	Gtk::Box container = Gtk::Box(Gtk::ORIENTATION_VERTICAL);

	// sync up queueing draw events for trace widgets
	bool on_timeout();

	std::function<void()> on_window_closed_callback;
	bool on_delete_event(GdkEventAny* any_event);
};
}  // namespace Keela

#endif  // TRACEWINDOW_H
