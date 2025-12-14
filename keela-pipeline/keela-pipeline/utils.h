//
// Created by brand on 6/4/2025.
//

#ifndef UTILS_H
#define UTILS_H
#include "bin.h"

namespace Keela {
template <typename Last>
inline void element_link_many(Last _) {
	spdlog::info("{} no more elements left to link", __func__);
}

template <typename First, typename Second, typename... Rest>
inline void element_link_many(First first, Second second, Rest... rest) {
	GstElement *f = first;
	GstElement *s = second;
	auto fname = gst_element_get_name(f);
	auto sname = gst_element_get_name(s);
	spdlog::debug("{} linking {} to {}", __func__, fname, sname);
	g_free(fname);
	g_free(sname);
	auto ret = gst_element_link(GST_ELEMENT(f), GST_ELEMENT(s));
	if(!ret) {
		throw std::runtime_error("failed to link elements");
	}
	Keela::element_link_many(second, rest...);
}

// TODO: make this generic over GstMiniObject* if needed
void delete_caps(GstCaps *caps);

template <typename T>
void delete_gobject(T *obj) {
	if(!G_IS_OBJECT(obj)) {
		spdlog::warn("{} can not release invalid object", __func__);
		return;
	}
	auto classname = G_OBJECT_CLASS_NAME(G_OBJECT_GET_CLASS(obj));
	// gobject doesn't like it if you try to unref an object with a refcount of 0
	auto refcount = GST_OBJECT_REFCOUNT(obj);
	spdlog::debug("{} {} refcount {}", __func__, classname, refcount);
	if(refcount > 0) {
		spdlog::info("{}: releasing managed ref of {}", __func__, classname);
		g_object_unref(obj);
	}
}
}  // namespace Keela

#endif  // UTILS_H
