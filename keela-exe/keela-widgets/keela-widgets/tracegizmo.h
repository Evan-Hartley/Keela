//
// Created by brand on 6/25/2025.
//

#ifndef TRACEGIZMO_H
#define TRACEGIZMO_H
#include <gtkmm/drawingarea.h>

#include "gizmocontrol.h"

namespace Keela {
class TraceGizmo : public Gtk::DrawingArea {
   public:
	TraceGizmo();

	~TraceGizmo() override;

	bool intersects(int x, int y) const;

	bool get_enabled() const;

   protected:
	bool on_button_press_event(GdkEventButton *button_event) override;

	bool on_button_release_event(GdkEventButton *release_event) override;

	bool on_motion_notify_event(GdkEventMotion *motion_event) override;

	bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr) override;

	void set_center(Gdk::Point center);

   private:
	std::unique_ptr<Gdk::Rectangle> bounds = nullptr;
	bool is_dragging = false;
	bool is_enabled = true;

	enum edit_mode { top_left, top_right, bottom_left, bottom_right, translate, none };

	edit_mode mode = none;

	std::unique_ptr<Keela::GizmoControl> ctrl_top_left = nullptr;
	std::unique_ptr<Keela::GizmoControl> ctrl_top_right = nullptr;
	std::unique_ptr<Keela::GizmoControl> ctrl_bottom_left = nullptr;
	std::unique_ptr<Keela::GizmoControl> ctrl_bottom_right = nullptr;
};
}  // namespace Keela

inline bool Keela::TraceGizmo::intersects(const int x, const int y) const {
	/*
	 * https://math.stackexchange.com/questions/76457/check-if-a-point-is-within-an-ellipse
	 * The region (disk) bounded by the ellipse is given by the equation:
	    ((x−h)^2/r_x^2) + ((y−k)^2/r_y^2) ≤1

	 So given a test point (x,y)
	 plug it in (1)
	 If the inequality is satisfied, then it is inside the ellipse;
	 otherwise it is outside the ellipse.
	 Moreover, the point is on the boundary of the region (i.e., on the ellipse) if and only if the inequality is
	 satisfied tightly (i.e., the left hand side evaluates to 1).
	 */

	auto r_x = HALF(bounds->get_width());
	auto r_y = HALF(bounds->get_height());

	// to avoid dividing by zero
	if(r_x == 0 || r_y == 0) {
		return false;
	}
	auto h = bounds->get_x() + r_x;
	auto k = bounds->get_y() + r_y;

	return (std::pow((x - h), 2.0) / std::pow(r_x, 2.0)) + (std::pow((y - k), 2.0) / std::pow(r_y, 2.0)) <= 1.0;
}
#endif  // TRACEGIZMO_H
