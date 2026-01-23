//
// Created by brand on 6/4/2025.
//

#ifndef UTILS_H
#define UTILS_H
#include <gstreamer-1.0/gst/gst.h>
#include <spdlog/spdlog.h>

#include "elementbase.h"

namespace Keela {

inline GstElement *to_gst_element(Keela::Element &e) {
	return e;
}

inline GstElement *to_gst_element(std::shared_ptr<Keela::Element> e) {
	return e->operator GstElement *();
}

template <typename Last>
inline void element_link_many(Last _) {
	spdlog::info("{} no more elements left to link", __func__);
}

template <typename First, typename Second, typename... Rest>
inline void element_link_many(First &first, Second &second, Rest &...rest) {
	GstElement *f = to_gst_element(first);
	GstElement *s = to_gst_element(second);
	auto fname = gst_element_get_name(f);
	auto sname = gst_element_get_name(s);
	spdlog::debug("{} linking {} to {}", __func__, fname, sname);

	auto ret = gst_element_link(GST_ELEMENT(f), GST_ELEMENT(s));
	if(!ret) {
		std::stringstream ss;
		ss << __func__ << ": failed to link " << fname << " to " << sname;
		g_free(fname);
		g_free(sname);
		throw std::runtime_error(ss.str());
	}
	g_free(fname);
	g_free(sname);
	Keela::element_link_many(second, rest...);
}

/// custom deleter to use for `GstBin*` managed by `std::shared_ptr`
void delete_bin(GstBin *bin);

/// custom deleter to use for `GstElement*` managed by `std::shared_ptr`
void delete_element(GstElement *e);

void delete_caps(GstCaps *caps);
}  // namespace Keela

#endif  // UTILS_H
