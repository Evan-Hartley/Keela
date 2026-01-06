//
// Created by brand on 1/5/2026.
//

#include "keela-pipeline/LeftShiftBin.h"

#include <gst/video/video-format.h>
#include <gst/video/video-info.h>

#include "keela-pipeline/caps.h"
#include "keela-pipeline/consts.h"
#include "keela-pipeline/queuebin.h"
#include "keela-pipeline/utils.h"
Keela::LeftShiftBin::LeftShiftBin() : Bin("LeftShiftBin") {
	LeftShiftBin::init();
	LeftShiftBin::link();
	set_enabled(left_shift);
}

Keela::LeftShiftBin::~LeftShiftBin() = default;

void Keela::LeftShiftBin::set_enabled(bool enabled) {
	left_shift = enabled;
	GstPad *pad = gst_element_get_static_pad(identity, "sink");
	if(left_shift) {
		SPDLOG_INFO("Installing data pad probe");
		data_probe_id =
		    gst_pad_add_probe(pad, static_cast<GstPadProbeType>(GST_PAD_PROBE_TYPE_BUFFER | GST_PAD_PROBE_TYPE_BLOCK),
		                      data_probe, this, nullptr);
	} else if(data_probe_id != 0) {
		SPDLOG_INFO("Removing data pad probe");
		gst_pad_remove_probe(pad, data_probe_id);
	}
	g_object_unref(pad);
}

void Keela::LeftShiftBin::init() {
	Caps caps = Caps();

	auto formats = {GRAY8, GRAY16_LE, GRAY16_BE};
	for(auto format : formats) {
		auto tmp_caps = Caps();
		tmp_caps.set_format(format);
		gst_caps_append(caps, tmp_caps);
	}
	g_object_set(caps_filter, "caps", static_cast<GstCaps *>(caps), nullptr);

	add_elements(caps_filter, identity);

	GstPad *pad = gst_element_get_static_pad(identity, "sink");
	gst_pad_add_probe(pad, static_cast<GstPadProbeType>(GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM | GST_PAD_PROBE_TYPE_BLOCK),
	                  caps_probe, this, nullptr);
	g_object_unref(pad);
}
void Keela::LeftShiftBin::link() {
	element_link_many(caps_filter, identity);
	add_ghost_pad(caps_filter, "sink");
	add_ghost_pad(identity, "src");
}
GstPadProbeReturn Keela::LeftShiftBin::data_probe(GstPad *pad, GstPadProbeInfo *info, gpointer user_data) {
	auto *self = static_cast<LeftShiftBin *>(user_data);

	assert(self->caps != nullptr);
	GstVideoInfo video_info;
	gst_video_info_init(&video_info);
	gst_video_info_from_caps(&video_info, *self->caps);
	int depth;
	auto structure = gst_caps_get_structure(*self->caps, 0);
	gst_structure_get_int(structure, "depth", &depth);
	auto format = video_info.finfo;

	if(g_strcmp0(format->name, GRAY8) == 0) {
		SPDLOG_WARN("data probe invoked for GRAY8 stream");
		SPDLOG_DEBUG("Removing data probe");
		self->data_probe_id = 0;
		return GST_PAD_PROBE_REMOVE;
	}
	auto buffer = gst_pad_probe_info_get_buffer(info);
	GstBufferMapInfo map_info;
	if(!gst_buffer_map(buffer, &map_info, GST_MAP_READWRITE)) {
		throw std::runtime_error("Failed to map buffer");
	};
	assert(map_info.flags == GST_MAP_READWRITE);
	assert(format->n_components == 1);
	assert(format->depth[0] == 16);

	/// number of bits to shift by
	auto shift = 16 - depth - 1;
	/// is the current pixel format little endian?
	auto format_is_le = (format->flags & GST_VIDEO_FORMAT_FLAG_LE) == GST_VIDEO_FORMAT_FLAG_LE;
	for(int y = 0; y < video_info.height; y++) {
		for(int x = 0; x < video_info.width; x++) {
			auto index = (x * sizeof(guint16)) + (y * video_info.stride[0]);
			/// does the endianness of the pixel format match the native endianness?
			bool byteswap = format_is_le && std::endian::native != std::endian::little;
			guint16 pixel = map_info.data[index];
			if(byteswap) {
				pixel = std::byteswap(pixel);
			}
			pixel <<= shift;
			if(byteswap) {
				pixel = std::byteswap(pixel);
			}
			*reinterpret_cast<guint16 *>(&map_info.data[index]) = pixel;
		}
	}
	gst_buffer_unmap(buffer, &map_info);
	return GST_PAD_PROBE_PASS;
}
GstPadProbeReturn Keela::LeftShiftBin::caps_probe(GstPad *pad, GstPadProbeInfo *info, gpointer user_data) {
	auto *self = static_cast<LeftShiftBin *>(user_data);
	if(!(GST_PAD_PROBE_INFO_TYPE(info) & GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM))
		return GST_PAD_PROBE_PASS;
	auto event = gst_pad_probe_info_get_event(info);
	if(GST_EVENT_TYPE(event) != GST_EVENT_CAPS)
		return GST_PAD_PROBE_PASS;
	SPDLOG_INFO("Got caps event");
	GstCaps *tmp_caps;
	gst_event_parse_caps(event, &tmp_caps);
	self->caps = std::make_shared<Caps>(tmp_caps);
	auto structure = gst_caps_get_structure(*self->caps, 0);
	auto format_name = gst_structure_get_string(structure, "format");
	if(g_strcmp0(format_name, GRAY8) != 0 && self->data_probe_id == 0) {
		self->set_enabled(self->left_shift);
	}
	return GST_PAD_PROBE_PASS;
}