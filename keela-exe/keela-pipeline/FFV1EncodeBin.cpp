//
// Created by brand on 1/7/26.
//

#include "keela-pipeline/FFV1EncodeBin.h"

#include "keela-pipeline/caps.h"
#include "keela-pipeline/consts.h"
#include "keela-pipeline/utils.h"

Keela::FFV1EncodeBin::FFV1EncodeBin() : Keela::Bin("FFV1EncodeBin") {
	SPDLOG_DEBUG("{}", __func__);
	FFV1EncodeBin::init();
	FFV1EncodeBin::link();
}

void Keela::FFV1EncodeBin::init() {
	auto formats = {GRAY8, GRAY16_LE, GRAY16_BE};
	Keela::Caps caps = Caps();
	for(auto format : formats) {
		auto tmp_caps = Caps();
		tmp_caps.set_format(format);
		gst_caps_append(caps, tmp_caps);
	}
	g_object_set(caps_filter, "caps", static_cast<GstCaps *>(caps), nullptr);
	add_elements(video_convert, caps_filter, enc);
}

void Keela::FFV1EncodeBin::link() {
	element_link_many(video_convert, caps_filter, enc);
	add_ghost_pad(video_convert, "sink");
	add_ghost_pad(enc, "src");
}
