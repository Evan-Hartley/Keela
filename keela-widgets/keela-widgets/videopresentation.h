#ifndef VIDEOPRESENTATION_H
#define VIDEOPRESENTATION_H

#include <gtkmm-3.0/gtkmm.h>

#include <memory>

#include "GLCameraRender.h"
#include "tracegizmo.h"

namespace Keela {
class VideoPresentation : public Gtk::Box {
   public:
	VideoPresentation(const std::string& label_text, std::shared_ptr<Keela::PresentationBin> presentation_bin,
	                  int width = 640, int height = 480);
	~VideoPresentation() override;

	void set_video_size(int width, int height);
	void add_overlay_widget(Gtk::Widget& widget);
	Gtk::Overlay& get_overlay();

   private:
	std::unique_ptr<GLCameraRender> gl_area;
	Gtk::Label label;
	Gtk::Overlay overlay;
};
}  // namespace Keela

#endif  // VIDEOPRESENTATION_H
