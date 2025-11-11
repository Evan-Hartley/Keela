//
// Created by brand on 7/7/2025.
//

#include "keela-pipeline/TraceBin.h"

#include <spdlog/spdlog.h>

#include "keela-pipeline/consts.h"
#include "keela-pipeline/gst-helpers.h"
#include "keela-pipeline/utils.h"

Keela::TraceBin::TraceBin() : QueueBin("TraceBin") {
	TraceBin::init();
	TraceBin::link();
}

Keela::TraceBin::TraceBin(const std::string &name) : QueueBin(name) {
	TraceBin::init();
	TraceBin::link();
}

Keela::TraceBin::~TraceBin() = default;

void Keela::TraceBin::enable_trace(bool do_enable) {
	auto max_buffers = do_enable ? 0 : 1;
	g_object_set(sink, "max-buffers", max_buffers, nullptr);

	g_object_set(sink, "drop", !do_enable, nullptr);
}

void Keela::TraceBin::set_trace_framerate(guint framerate) {
	spdlog::info("Setting trace framerate to {} fps", framerate);

	// Create caps with the specified framerate
	trace_caps = Caps();
	trace_caps.set_framerate(framerate, 1);

	// Apply the caps to the capsfilter
	g_object_set(caps_filter, "caps", static_cast<GstCaps *>(trace_caps), nullptr);
}

void Keela::TraceBin::init() {
	add_elements(video_rate, caps_filter, sink);
	enable_trace(false);

	// Set default trace framerate
	set_trace_framerate(DEFAULT_TRACE_FPS);
}

void Keela::TraceBin::link() {
	// Link: videorate -> capsfilter -> appsink
	element_link_many(video_rate, caps_filter, sink);
	link_queue(video_rate);
}
