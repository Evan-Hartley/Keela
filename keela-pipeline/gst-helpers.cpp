//
// Created by brand on 5/28/2025.
//

#include "keela-pipeline/gst-helpers.h"

#include <spdlog/spdlog.h>

#include <stdexcept>

gint Keela::gst_enum_variant_by_nick(GObject *obj, const std::string &propname, const std::string &nickname) {
	auto cls = G_OBJECT_GET_CLASS(obj);
	auto propspec = g_object_class_find_property(cls, propname.c_str());
	if(!propspec || !G_IS_PARAM_SPEC_ENUM(propspec)) {
		throw std::invalid_argument("Property " + propname + " is not an enum");
	}

	auto enum_type = propspec->value_type;
	auto enum_class = G_ENUM_CLASS(g_type_class_ref(enum_type));
	auto enum_value = g_enum_get_value_by_nick(enum_class, nickname.c_str());

	if(enum_value) {
		const gint value = enum_value->value;
		spdlog::debug("{}\t{}::{} -> {}", __func__, propname, nickname, value);
		g_type_class_unref(enum_class);
		return value;
	} else {
		g_type_class_unref(enum_class);
		throw std::invalid_argument("Could not find variant " + nickname + " of " + propname);
	}
}
