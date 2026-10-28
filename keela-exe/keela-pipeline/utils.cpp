//
// Created by brand on 6/5/2025.
//
#include "keela-pipeline/utils.h"

void Keela::delete_bin(GstBin *bin) {
	if(!GST_IS_ELEMENT(bin)) {
		spdlog::warn("{} can not release invalid element", __func__);
		return;
	}
	auto refcount = GST_OBJECT_REFCOUNT(bin);
	auto name = GST_OBJECT_NAME(bin);
	spdlog::debug("{} {} refcount {}", __func__, name, refcount);
	if(refcount > 0)
		g_object_unref(bin);
}

void Keela::delete_element(GstElement *e) {
	if(!GST_IS_ELEMENT(e)) {
		spdlog::warn("{} can not release invalid element", __func__);
		return;
	}
	auto refcount = GST_OBJECT_REFCOUNT(e);
	auto name = GST_OBJECT_NAME(e);
	spdlog::debug("{} {} refcount {}", __func__, name, refcount);
	if(refcount > 0)
		g_object_unref(e);
}

void Keela::delete_caps(GstCaps *caps) {
	if(!GST_IS_CAPS(caps)) {
		spdlog::warn("{} can not release invalid element", __func__);
		return;
	}
	auto refcount = GST_OBJECT_REFCOUNT(caps);
	spdlog::debug("{} refcount {}", __func__, refcount);
	if(refcount > 0)
		gst_caps_unref(caps);
}
