//
// Created by brand on 5/26/2025.
//

#include "keela-pipeline/presentationbin.h"

#include <keela-pipeline/gtkglsink.h>
#include <keela-pipeline/gtksink.h>
#include <spdlog/spdlog.h>

#include <stdexcept>

#include "keela-pipeline/utils.h"

Keela::PresentationBin::PresentationBin(const std::string &name) : QueueBin(name) {
	spdlog::info("{}", __func__);
	PresentationBin::init();
	gboolean ret = false;
	ret = gst_element_set_name(GST_OBJECT(static_cast<GstElement *>(video_rate)), (name + "_videorate").c_str());
	// TODO: name sink
	if(!ret) {
		spdlog::warn("{} Failed to name elements", __func__);
	}
	PresentationBin::link();
}

Keela::PresentationBin::PresentationBin() : QueueBin() {
	spdlog::info("{}", __func__);
	PresentationBin::init();
	PresentationBin::link();
}

Keela::PresentationBin::~PresentationBin() {
	spdlog::debug(__func__);
}

void Keela::PresentationBin::set_presentation_framerate(const guint framerate) {
	// we do not want to inherit the old caps
	presentation_caps = Caps();
	presentation_caps.set_framerate(60, 1);
	g_object_set(caps_filter, "caps", static_cast<GstCaps *>(presentation_caps), nullptr);
}

void Keela::PresentationBin::init() {
	g_object_set(sink, "max-buffers", 1, nullptr);
	g_object_set(sink, "drop", true, nullptr);
}

void Keela::PresentationBin::link() {
	set_presentation_framerate(60);
	add_elements(video_rate, caps_filter, sink);
	element_link_many(video_rate, caps_filter, sink);
	link_queue(video_rate);
	// add_ghost_pad(video_rate, "sink");
}
