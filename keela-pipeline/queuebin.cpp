//
// Created by brand on 5/28/2025.
//

#include "keela-pipeline/queuebin.h"

#include <gstreamer-1.0/gst/gst.h>
#include <spdlog/spdlog.h>

#include <stdexcept>

Keela::QueueBin::QueueBin() {
	spdlog::info("{}", __func__);
	QueueBin::init();
	QueueBin::link();
}

Keela::QueueBin::~QueueBin() {
	spdlog::debug("{}", __func__);
}

Keela::QueueBin::QueueBin(const std::string &name) : Bin(name) {
	spdlog::info("{}", __func__);
	QueueBin::init();
	gboolean ret;
	ret = gst_element_set_name(GST_ELEMENT(static_cast<GstElement *>(queue)), (name + "_queue").c_str());
	if(!ret) {
		spdlog::warn("{} Failed to name queue element", __func__);
	}
	QueueBin::link();
}

void Keela::QueueBin::link_queue(GstElement *sink) const {
	auto name = gst_element_get_name(sink);
	spdlog::info("Linking internal queue to {}", name);
	g_free(name);
	if(!gst_element_link(queue, sink)) {
		throw std::runtime_error("Failed to link queue");
	}
}

void Keela::QueueBin::init() {
	// queue = gst_element_factory_make("queue", nullptr);
	if(!queue) {
		throw std::runtime_error("Failed to create queue element");
	}
	spdlog::info("Successfully created queue element");
}

void Keela::QueueBin::link() {
	// GstElement *b = *this;
	add_elements(queue);
	// if (!gst_bin_add(GST_BIN(b), queue)) {
	//     throw std::runtime_error("Failed to add queue to bin");
	// }
	add_ghost_pad(queue, "sink");
}
