#include "keela-widgets/videopresentation.h"

Keela::VideoPresentation::VideoPresentation(const std::string &label_text,
                                            std::shared_ptr<Keela::PresentationBin> presentation_bin,
                                            Keela::IControlGLCameraRenderHeatmap &controller, int width, int height)
    : Gtk::Box(Gtk::ORIENTATION_VERTICAL),
      gl_area(std::make_unique<GLCameraRender>(presentation_bin, controller)),
      label(label_text),
      m_base_width(width),
      m_base_height(height),
      m_width(width),
      m_height(height) {
	set_spacing(5);
	set_border_width(5);

	gl_area->set_size_request(width, height);

	overlay.add(*gl_area);

	pack_start(label, false, false, 5);
	pack_start(overlay, false, false, 5);

	show_all_children();
}

Keela::VideoPresentation::~VideoPresentation() = default;

std::pair<int, int> Keela::VideoPresentation::get_size() const {
	return {m_width, m_height};
}

void Keela::VideoPresentation::set_presentation_size(int width, int height) {
	m_width = width;
	m_height = height;
	gl_area->set_size_request(width, height);
}

void Keela::VideoPresentation::reset_dimensions() {
	m_width = m_base_width;
	m_height = m_base_height;
	gl_area->set_size_request(m_width, m_height);
}

void Keela::VideoPresentation::swap_dimensions() {
	std::swap(m_width, m_height);
	gl_area->set_size_request(m_width, m_height);
}

void Keela::VideoPresentation::add_overlay_widget(Gtk::Widget &widget) {
	overlay.add_overlay(widget);
}

Gtk::Overlay &Keela::VideoPresentation::get_overlay() {
	return overlay;
}
