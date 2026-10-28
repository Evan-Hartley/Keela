//
// Created by brand on 6/25/2025.
//

#include <gtest/gtest.h>
#include <gtkmm-3.0/gtkmm.h>
TEST(Rect, Intersects) {
	// this demonstrates that it may be sufficient to emulate a point as a 1x1 rect for intersection calculations
	Gdk::Rectangle bounds = Gdk::Rectangle(0, 0, 100, 100);
	Gdk::Rectangle point = Gdk::Rectangle(0, 0, 1, 1);

	ASSERT_TRUE(bounds.intersects(point));
}

TEST(Rect, CanHaveNegativeDimensions) {
	Gdk::Rectangle _ = Gdk::Rectangle(0, 0, -100, -100);
}
