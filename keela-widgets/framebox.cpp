//
// Created by brand on 5/26/2025.
//

#include "keela-widgets/framebox.h"

#include <iostream>

Keela::FrameBox::FrameBox() {
	Gtk::Frame::add(m_box);
}

Keela::FrameBox::FrameBox(const std::string& title, Gtk::Orientation orientation) : FrameBox() {
	set_label(title);
	m_box.set_orientation(orientation);
}

void Keela::FrameBox::add(Widget& widget) {
	m_box.add(widget);
}

void Keela::FrameBox::set_spacing(const int i) {
	m_box.set_spacing(i);
}
