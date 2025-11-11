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

/// custom deleter to use for `GstBin*` managed by `std::shared_ptr`
void delete_bin(GstBin *bin);

/// custom deleter to use for `GstElement*` managed by `std::shared_ptr`
void delete_element(GstElement *e);

void delete_caps(GstCaps *caps);
}  // namespace Keela

#endif  // UTILS_H
