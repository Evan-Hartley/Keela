//
// Created by brand on 5/31/2025.
//

#include "keela-pipeline/gtksink.h"

#include <spdlog/spdlog.h>

#include <stdexcept>

Keela::GtkSink::GtkSink() : SimpleElement("gtksink") {
	spdlog::info("{}", __func__);
}

Keela::GtkSink::GtkSink(const std::string &name) : GtkSink() {
	gst_element_set_name(m_element.get(), name.c_str());
}

Keela::GtkSink::~GtkSink() {
}

gpointer Keela::GtkSink::get_widget() {
	GObject *o = nullptr;
	g_object_get(m_element.get(), "widget", &o, nullptr);
	if(!o) {
		throw std::runtime_error("Could not get widget");
	}
	spdlog::info("Got widget");
	return o;
}
