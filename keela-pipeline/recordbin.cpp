//
// Created by brand on 5/26/2025.
//

#include "keela-pipeline/recordbin.h"

#include <gst/video/video-info.h>
#include <spdlog/spdlog.h>

#include <stdexcept>

#include "keela-pipeline/caps.h"
#include "keela-pipeline/gst-helpers.h"
#include "keela-pipeline/h264EncodeBin.h"
#include "keela-pipeline/utils.h"
Keela::RecordBin::RecordBin(const std::string &name)
    : QueueBin(name), mux("matroskamux", name + "_mux"), sink("filesink", name + "_sink") {
	SPDLOG_DEBUG("{}", __func__);
	RecordBin::init();

	RecordBin::link();
}

void Keela::RecordBin::link() {
	// TODO: is there a way to prevent a narrowing color format conversion if avoidable?
	// NOTE: if encoding using GRAY16_LE, conv seems to pick Y444; is this a narrowing conversion?

	element_link_many(enc, mux, sink);
	link_queue(enc);
}

void Keela::RecordBin::init() {
#ifdef KEELA_USE_FFV1
	enc = std::make_shared<FFV1EncodeBin>();
#endif
#ifndef KEELA_USE_FFV1
	enc = std::make_shared<H264EncodeBin>();
#endif

	add_elements(enc, mux, sink);
	// this will automatically offset incoming camera streams to start at offset 0
	g_object_set(mux, "offset-to-zero", true, nullptr);
}

void Keela::RecordBin::set_directory(const std::string &full_filename) {
	// try not to call this method once recording has started
	g_object_set(sink, "location", full_filename.c_str(), nullptr);
}

Keela::RecordBin::~RecordBin() {
	SPDLOG_DEBUG(__func__);
}
