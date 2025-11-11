//
// Created by brand on 5/26/2025.
//

#include "keela-pipeline/recordbin.h"

#include <spdlog/spdlog.h>

#include <stdexcept>

#include "keela-pipeline/gst-helpers.h"
#include "keela-pipeline/utils.h"

using namespace spdlog;

Keela::RecordBin::RecordBin(const std::string &name) : QueueBin(name) {
	spdlog::info("{}", __func__);
	RecordBin::init();
	gboolean ret = false;

	ret = gst_object_set_name(GST_OBJECT(static_cast<GstElement *>(this->enc)), (name + "_enc").c_str());
	ret &= gst_object_set_name(GST_OBJECT(static_cast<GstElement *>(mux)), (name + "_mux").c_str());
	ret &= gst_object_set_name(GST_OBJECT(static_cast<GstElement *>(sink)), (name + "_sink").c_str());
	if(!ret) {
		throw std::runtime_error("Failed to name Elements");
	}
	RecordBin::link();
}

void Keela::RecordBin::link() {
	add_elements(enc, mux, sink);
	element_link_many(enc, mux, sink);
	link_queue(enc);
}

void Keela::RecordBin::init() {
	g_object_set(enc, "quantizer", 0, nullptr);
	auto variant = gst_enum_variant_by_nick(G_OBJECT(static_cast<GstElement *>(enc)), "pass", "quant");
	g_object_set(enc, "pass", variant, nullptr);
}

Keela::RecordBin::RecordBin() : QueueBin() {
	spdlog::info("{}", __func__);
	RecordBin::init();
	RecordBin::link();
}

void Keela::RecordBin::set_directory(const std::string &full_filename) {
	// try not to call this method once recording has started
	g_object_set(sink, "location", full_filename.c_str(), nullptr);
}

Keela::RecordBin::~RecordBin() {
	spdlog::debug(__func__);
}
