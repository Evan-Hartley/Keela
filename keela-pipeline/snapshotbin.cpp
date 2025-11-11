//
// Created by brand on 6/10/2025.
//

#include "keela-pipeline/snapshotbin.h"

#include <sstream>

#include "keela-pipeline/gst-helpers.h"
#include "keela-pipeline/utils.h"

Keela::SnapshotBin::SnapshotBin(const std::string &name) : QueueBin(name) {
	SnapshotBin::init();
	SnapshotBin::link();
}

Keela::SnapshotBin::~SnapshotBin() {
	spdlog::debug(__func__);
}

void Keela::SnapshotBin::take_snapshot() {
	spdlog::info(__func__);
	GstSample *sample;
	g_signal_emit_by_name(app_sink, "pull-sample", &sample);

	if(!sample) {
		throw std::runtime_error("Failed to get sample from appsink");
	}
	// TODO: save the snapshot data to disk
	gst_sample_unref(sample);
}

void Keela::SnapshotBin::init() {
	g_object_set(app_sink, "max-buffers", 1, nullptr);
	g_object_set(app_sink, "drop", true, nullptr);
}

void Keela::SnapshotBin::link() {
	add_elements(app_sink);
	link_queue(app_sink);
}
