//
// Created by brand on 6/25/2025.
//

#include "keela-widgets/tracegizmo.h"

#include <spdlog/spdlog.h>

Keela::TraceGizmo::TraceGizmo() {
	add_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::BUTTON_PRESS_MASK);
}

Keela::TraceGizmo::~TraceGizmo() {
}

bool Keela::TraceGizmo::on_button_press_event(GdkEventButton *button_event) {
	spdlog::debug(__func__);
	Gdk::Point point(button_event->x, button_event->y);

	if(bounds == nullptr) {
		spdlog::info("{}: creating region of interest");
		bounds = std::make_unique<Gdk::Rectangle>(point.get_x(), point.get_y(), 0, 0);

		ctrl_top_left = std::make_unique<GizmoControl>(point.get_x(), point.get_y());
		ctrl_top_right = std::make_unique<GizmoControl>(point.get_x(), point.get_y());
		ctrl_bottom_left = std::make_unique<GizmoControl>(point.get_x(), point.get_y());
		ctrl_bottom_right = std::make_unique<GizmoControl>(point.get_x(), point.get_y());

		ctrl_top_left->set_peer_y(*ctrl_top_right);
		ctrl_top_right->set_peer_y(*ctrl_top_left);
		ctrl_bottom_left->set_peer_y(*ctrl_bottom_right);
		ctrl_bottom_right->set_peer_y(*ctrl_bottom_left);

		ctrl_top_left->set_peer_x(*ctrl_bottom_left);
		ctrl_bottom_left->set_peer_x(*ctrl_top_left);
		ctrl_top_right->set_peer_x(*ctrl_bottom_right);
		ctrl_bottom_right->set_peer_x(*ctrl_top_right);
	}

	if(ctrl_top_left->intersects(point)) {
		mode = top_left;
	} else if(ctrl_top_right->intersects(point)) {
		mode = top_right;
	} else if(ctrl_bottom_left->intersects(point)) {
		mode = bottom_left;
	} else if(ctrl_bottom_right->intersects(point)) {
		mode = bottom_right;
	} else if(intersects(point.get_x(), point.get_y())) {
		mode = translate;
	} else {
		mode = none;
	}
	return DrawingArea::on_button_press_event(button_event);
}

bool Keela::TraceGizmo::on_button_release_event(GdkEventButton *release_event) {
	spdlog::debug(__func__);
	mode = none;
	return DrawingArea::on_button_release_event(release_event);
}

bool Keela::TraceGizmo::on_motion_notify_event(GdkEventMotion *motion_event) {
	Gdk::Point pt(motion_event->x, motion_event->y);

	// ignore any motion events that are outside the window bounds
	if(pt.get_x() > get_width() || pt.get_y() > get_height() || pt.get_x() < 0 || pt.get_y() < 0) {
		return DrawingArea::on_motion_notify_event(motion_event);
	}

	// all of our pointers should be initialized in order to proceed
	if(ctrl_top_left == nullptr || ctrl_top_right == nullptr || ctrl_bottom_left == nullptr ||
	   ctrl_bottom_right == nullptr || bounds == nullptr) {
		return DrawingArea::on_motion_notify_event(motion_event);
	}

	// update "hovered" status of each control element
	ctrl_top_left->set_hovered((ctrl_top_left->intersects(pt) & (mode == none)) || mode == top_left);
	ctrl_top_right->set_hovered((ctrl_top_right->intersects(pt) & (mode == none)) || mode == top_right);
	ctrl_bottom_left->set_hovered((ctrl_bottom_left->intersects(pt) & (mode == none)) || mode == bottom_left);
	ctrl_bottom_right->set_hovered((ctrl_bottom_right->intersects(pt) & (mode == none)) || mode == bottom_right);

	// find which control is currently being pointed at
	GizmoControl *control = nullptr;
	switch(mode) {
		case top_left:
			control = ctrl_top_left.get();
			control->set_center(pt);
			break;
		case top_right:
			control = ctrl_top_right.get();
			control->set_center(pt);
			break;
		case bottom_left:
			control = ctrl_bottom_left.get();
			control->set_center(pt);
			break;
		case bottom_right:
			control = ctrl_bottom_right.get();
			control->set_center(pt);
			break;
		case translate:
			set_center(pt);
			return DrawingArea::on_motion_notify_event(motion_event);
			break;
		case none:
			// if there are no edits being made, just skip
			return DrawingArea::on_motion_notify_event(motion_event);
	}

	bounds->set_x(ctrl_top_left->get_center().get_x());
	bounds->set_y(ctrl_top_left->get_center().get_y());

	auto width = ctrl_top_right->get_x() - ctrl_top_left->get_x();
	auto height = ctrl_bottom_left->get_y() - ctrl_top_left->get_y();
	bounds->set_width(width);
	bounds->set_height(height);
	return DrawingArea::on_motion_notify_event(motion_event);
}

bool Keela::TraceGizmo::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
	if(bounds == nullptr) {
		return Gtk::DrawingArea::on_draw(cr);
	}
	cr->save();
	cr->set_source_rgb(0, 0, 1);
	cr->set_line_width(2);
	if(ctrl_top_left != nullptr && ctrl_top_right != nullptr && ctrl_bottom_left != nullptr &&
	   ctrl_bottom_right != nullptr) {
		ctrl_top_left->draw(cr);
		ctrl_top_right->draw(cr);
		ctrl_bottom_left->draw(cr);
		ctrl_bottom_right->draw(cr);
	}

	if(bounds->get_width() != 0 && bounds->get_height() != 0) {
		cr->save();
		cr->translate(bounds->get_x() + HALF(bounds->get_width()), bounds->get_y() + HALF(bounds->get_height()));
		cr->scale(HALF(bounds->get_width()), HALF(bounds->get_height()));
		cr->translate(-HALF(bounds->get_x()), -HALF(bounds->get_y()));
		cr->arc(HALF(bounds->get_x()), HALF(bounds->get_y()), 1, 0, 2 * M_PI);
		cr->restore();
		cr->stroke();
	}

	cr->restore();

	return DrawingArea::on_draw(cr);
}

void Keela::TraceGizmo::set_center(Gdk::Point center) {
	auto x = bounds->get_x() + bounds->get_width() / 2;
	auto y = bounds->get_y() + bounds->get_height() / 2;

	auto delta_x = x - center.get_x();
	auto delta_y = y - center.get_y();

	bounds->set_x(bounds->get_x() - delta_x);
	bounds->set_y(bounds->get_y() - delta_y);

	assert(bounds->get_x() + bounds->get_width() / 2 == center.get_x());
	assert(bounds->get_y() + bounds->get_height() / 2 == center.get_y());
	Gdk::Point pt;
	pt.set_x(bounds->get_x());
	pt.set_y(bounds->get_y());
	ctrl_top_left->set_center(pt, false);
	pt.set_x(bounds->get_x() + bounds->get_width());
	ctrl_top_right->set_center(pt, false);
	pt.set_y(bounds->get_y() + bounds->get_height());
	ctrl_bottom_right->set_center(pt, false);
	pt.set_x(bounds->get_x());
	ctrl_bottom_left->set_center(pt, false);
}

bool Keela::TraceGizmo::get_enabled() const {
	return bounds != nullptr;
}
