//
// Created by brand on 5/31/2025.
//

#include "keela-pipeline/gtkglsink.h"

#include <spdlog/spdlog.h>

Keela::GtkGlSink::GtkGlSink() {
	spdlog::info("{}", __func__);
	init();
	link();
}

Keela::GtkGlSink::GtkGlSink(const std::string &name) : Bin(name) {
	spdlog::info("{}", __func__);
	init();
	gboolean ret = false;
	ret = gst_element_set_name(GST_ELEMENT(static_cast<GstElement *>(glsink)), (name + "_gl").c_str());
	ret |= gst_element_set_name(GST_ELEMENT(static_cast<GstElement *>(glsink)), (name + "_gtkgl").c_str());
	if(!ret) {
		spdlog::warn("Could not set name of elements");
	}
	link();
}

Keela::GtkGlSink::~GtkGlSink() {
}

void Keela::GtkGlSink::init() {
	/*
	glsink = gst_element_factory_make("glsinkbin", NULL);
	gtkglsink = gst_element_factory_make("gtkglsink", NULL);*/
	// if (glsink && gtkglsink) {
	spdlog::info("Successfully created Gtk OpenGL elements");
	g_object_set(glsink, "sink", gtkglsink, NULL);
	// } else {
	//   throw std::runtime_error("Failed to create Gtk OpenGL element");
	//}
}

void Keela::GtkGlSink::link() {
	GstElement *b = *this;
	gst_bin_add_many(GST_BIN(b), glsink, gtkglsink, NULL);
	add_ghost_pad(glsink, "sink");
}

gpointer Keela::GtkGlSink::get_widget() {
	GObject *o = nullptr;
	g_object_get(glsink, "widget", &o, nullptr);
	if(!o) {
		throw std::runtime_error("Could not get GTK widget");
	}
	spdlog::info("Got GTK widget");
	return o;
}
