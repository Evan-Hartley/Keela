//
// Created by brand on 5/26/2025.
//

#include "keela-pipeline/bin.h"

#include <spdlog/spdlog.h>

#include <sstream>
#include <stdexcept>

#include "keela-pipeline/utils.h"

Keela::Bin::Bin(const std::string &name) : Bin() {
	spdlog::info("Naming bin {}", name);
	if(!gst_object_set_name(GST_OBJECT(bin.get()), name.c_str())) {
		spdlog::warn("Could not set name of bin");
	}
}

Keela::Bin::Bin() {
	spdlog::info("{} {}", typeid(*this).name(), __func__);
	auto b = GST_BIN(gst_bin_new(nullptr));
	if(b == nullptr) {
		throw std::runtime_error("Failed to create bin");
	}
	bin = std::shared_ptr<GstBin>(b, Keela::delete_bin);
}

Keela::Bin::~Bin() {
	spdlog::trace("{}", __func__);
}

Keela::Bin::operator struct _GstElement *() const {
	const gchar *tname = g_type_name(G_OBJECT_TYPE(bin.get()));
	const gchar *name = GST_ELEMENT_NAME(GST_ELEMENT(bin.get()));
	if(!name) {
		spdlog::trace("{} Type name {}", __func__, tname);
	} else {
		spdlog::trace("{}::{} Type name {}", name, __func__, tname);
	}
	return GST_ELEMENT(bin.get());
}

Keela::Bin::operator struct _GstBin *() const {
	const gchar *tname = g_type_name(G_OBJECT_TYPE(bin.get()));
	const gchar *name = GST_ELEMENT_NAME(GST_ELEMENT(bin.get()));
	if(!name) {
		spdlog::trace("{} Type name {}", __func__, tname);
	} else {
		spdlog::trace("{}::{} Type name {}", name, __func__, tname);
	}
	return GST_BIN(bin.get());
}

void Keela::Bin::dump_bin_graph() const {
	const gchar *name = GST_ELEMENT_NAME(bin.get());
	std::stringstream ss;
	ss << name << ".dot";
	gst_debug_bin_to_dot_file(bin.get(), GST_DEBUG_GRAPH_SHOW_ALL, ss.str().c_str());
}

void Keela::Bin::add_ghost_pad(GstElement *element, const std::string &pad_name) const {
	spdlog::debug("{} {}", __func__, pad_name);
	GstObject *parent = gst_object_get_parent(GST_OBJECT(element));
	GstElement *bin = *this;
	if(GST_BIN(parent) != GST_BIN(bin)) {
		throw std::runtime_error("Element is not owned by this bin");
	}
	gst_object_unref(GST_OBJECT(parent));

	GstPad *target = gst_element_get_static_pad(element, pad_name.c_str());
	if(!target) {
		throw std::runtime_error("Failed to get target pad");
	}
	GstPad *ghost = gst_ghost_pad_new(nullptr, target);
	if(!ghost) {
		throw std::runtime_error("Failed to create ghost pad");
	}
	gst_object_unref(target);
	gst_element_add_pad(*this, ghost);
}
