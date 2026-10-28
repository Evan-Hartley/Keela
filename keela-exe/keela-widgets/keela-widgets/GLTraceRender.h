//
// Created by brand on 6/30/2025.
//

#ifndef GLTRACERENDER_H
#define GLTRACERENDER_H
#include <gtkmm/box.h>
#include <gtkmm/glarea.h>
#include <gtkmm/label.h>

#include "cameramanager.h"
#include "glad/glad.h"
#include "keela-pipeline/consts.h"
#include "tracegizmo.h"

namespace Keela {
/**
 * Abstract base class: A source that can be rendered in the trace window
 */
class ITraceable {
   public:
	virtual ~ITraceable() = default;

	virtual std::pair<int, int> get_display_size() const = 0;

	virtual std::shared_ptr<TraceBin> get_trace_bin() = 0;

	virtual std::shared_ptr<TraceGizmo> get_trace_gizmo() = 0;

	virtual std::string get_name() = 0;
};

class GLTraceRender final : public Gtk::Box {
   public:
	explicit GLTraceRender(const std::shared_ptr<ITraceable> &cam_to_trace);

	~GLTraceRender() override;

	void set_trace_render_framerate(double framerate);

	void set_plot_duration_sec(int duration_sec);

	void update_trace_buffer_length();

	void clear_buffer();

   private:
	Gtk::GLArea gl_area;
	Gtk::Label name_label;
	Gtk::Label min_label;
	Gtk::Label max_label;
	// Some experiments use fluorescence that is stronger at lower values
	// this checkbox allows inverting the y axis for better visualization of the "real" signal
	Gtk::CheckButton invert_y_axis_check;

	// *basically* a pointer to the camera control window
	std::shared_ptr<Keela::ITraceable> trace;

	unsigned int shader_program{};

	void on_gl_realize();

	bool on_gl_render(const Glib::RefPtr<Gdk::GLContext> &context);

	unsigned int VAO = -1;
	unsigned int VBO = -1;

	std::string vertex_shader_source;
	std::string fragment_shader_source;

	std::jthread worker_thread;
	std::mutex worker_mutex;
	std::deque<float> plot_points;

	int plot_duration_sec = 10;
	int trace_framerate = DEFAULT_TRACE_FPS;
	/**
	 * target length of plot_points buffer. Should equal plot_duration_sec * framerate
	 */
	unsigned long long plot_length = plot_duration_sec * trace_framerate;
	float plot_max = 255;
	float plot_min = 0;

	/**
	 * function to be used in worker_thread in order to process video data
	 * @param token
	 */
	void process_video_data(const std::stop_token &token);

	template <typename T>
	double calculate_roi_average(GstSample *sample, GstStructure *structure, std::endian endianness);
};
}  // namespace Keela
#endif  // GLTRACERENDER_H
