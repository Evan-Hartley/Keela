//
// Created by brand on 6/5/2025.
//
#include "keela-pipeline/utils.h"

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
