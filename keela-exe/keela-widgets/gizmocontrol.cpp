//
// Created by brand on 6/26/2025.
//

#include "keela-widgets/gizmocontrol.h"

#include <spdlog/spdlog.h>

Keela::GizmoControl::GizmoControl() : Gdk::Rectangle(0, 0, width, width) {
}

Keela::GizmoControl::GizmoControl(const int x, const int y)
    : Gdk::Rectangle(x - HALF(width), y - HALF(width), width, width) {
}

Keela::GizmoControl::~GizmoControl() {
}

void Keela::GizmoControl::draw(const Cairo::RefPtr<Cairo::Context> &cr) const {
	cr->save();

	cr->rectangle(get_x(), get_y(), get_width(), get_height());
	if(hovered) {
		cr->fill();
	} else {
		cr->stroke();
	}
	cr->restore();
}

bool Keela::GizmoControl::intersects(const Gdk::Point &position) const {
	auto rect = Gdk::Rectangle(position.get_x(), position.get_y(), 1, 1);
	return Gdk::Rectangle::intersects(rect);
}

void Keela::GizmoControl::set_hovered(const bool hovered) {
	this->hovered = hovered;
}

void Keela::GizmoControl::set_center(Gdk::Point center, bool update_peer) {
	auto x = center.get_x() - HALF(width);
	auto y = center.get_y() - HALF(width);

	set_x(x);
	set_y(y);
	// I do not need to convert coordinates to center since all rectangles origin on the top left
	peer_x->set_x(x);
	peer_y->set_y(y);
}

Gdk::Point Keela::GizmoControl::get_center() const {
	auto x = get_x() + HALF(width);
	auto y = get_y() + HALF(width);
	return Gdk::Point(x, y);
}

void Keela::GizmoControl::set_peer_x(GizmoControl &peer) {
	peer_x = &peer;
}

void Keela::GizmoControl::set_peer_y(GizmoControl &peer) {
	peer_y = &peer;
}
