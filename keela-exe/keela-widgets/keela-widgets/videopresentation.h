#ifndef VIDEOPRESENTATION_H
#define VIDEOPRESENTATION_H

#include <gtkmm-3.0/gtkmm.h>

#include <memory>

#include "GLCameraRender.h"
#include "keela-pipeline/consts.h"
#include "tracegizmo.h"

namespace Keela {
class VideoPresentation : public Gtk::Box {
   public:
	VideoPresentation(const std::string &label_text, std::shared_ptr<Keela::PresentationBin> presentation_bin,
	                  IControlGLCameraRenderHeatmap &controller, int width = DEFAULT_PRESENTATION_WIDTH,
	                  int height = DEFAULT_PRESENTATION_HEIGHT);

	~VideoPresentation() override;

	std::pair<int, int> get_size() const;

	void set_presentation_size(int width, int height);

	void reset_dimensions();

	void swap_dimensions();

	void add_overlay_widget(Gtk::Widget &widget);

	Gtk::Overlay &get_overlay();

   private:
	std::unique_ptr<GLCameraRender> gl_area;
	Gtk::Label label;
	Gtk::Overlay overlay;
	int m_base_width;
	int m_base_height;
	int m_width;
	int m_height;
};
}  // namespace Keela

#endif  // VIDEOPRESENTATION_H
