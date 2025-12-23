//
// Created by brand on 5/26/2025.
//

#include "keela-pipeline/recordbin.h"

#include <gst/video/video-info.h>
#include <spdlog/spdlog.h>

#include <stdexcept>

#include "keela-pipeline/caps.h"
#include "keela-pipeline/gst-helpers.h"
#include "keela-pipeline/utils.h"

Keela::RecordBin::RecordBin(const std::string &name) : QueueBin(name) {
	spdlog::info("{}", __func__);
	RecordBin::init();
	gboolean ret = false;

	ret = gst_object_set_name(GST_OBJECT(static_cast<GstElement *>(this->enc)), (name + "_enc").c_str());
	ret &= gst_object_set_name(GST_OBJECT(static_cast<GstElement *>(mux)), (name + "_mux").c_str());
	ret &= gst_object_set_name(GST_OBJECT(static_cast<GstElement *>(sink)), (name + "_sink").c_str());
	if(!ret) {
		throw std::runtime_error("Failed to name Elements");
	}
	RecordBin::link();
}

void Keela::RecordBin::link() {
	// TODO: is there a way to prevent a narrowing color format conversion if avoidable?
	// NOTE: if encoding using GRAY16_LE, conv seems to pick Y444; is this a narrowing conversion?
	add_elements(conv, caps_filter, enc, mux, sink);
	element_link_many(conv, caps_filter, enc, mux, sink);
	link_queue(conv);
}

void Keela::RecordBin::init() {
	g_object_set(enc, "quantizer", 0, nullptr);
	auto variant = gst_enum_variant_by_nick(G_OBJECT(static_cast<GstElement *>(enc)), "pass", "quant");
	g_object_set(enc, "pass", variant, nullptr);
	auto pad = gst_element_get_static_pad(queue, "src");
	gst_pad_add_probe(pad, static_cast<GstPadProbeType>(GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM | GST_PAD_PROBE_TYPE_BLOCK),
	                  queue_caps_probe, this, nullptr);
	gst_object_unref(pad);
}

GstPadProbeReturn Keela::RecordBin::queue_caps_probe(GstPad *pad, GstPadProbeInfo *info, void *user_data) {
	if(!(GST_PAD_PROBE_INFO_TYPE(info) & GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM))
		return GST_PAD_PROBE_PASS;
	auto event = gst_pad_probe_info_get_event(info);
	if(GST_EVENT_TYPE(event) == GST_EVENT_CAPS) {
		spdlog::info("{}: got caps event", __func__);
		GstCaps *caps;
		gst_event_parse_caps(event, &caps);  // does this leak?
		auto structure = gst_caps_get_structure(caps, 0);
		gint depth;
		if(!gst_structure_get_int(structure, "depth", &depth)) {
			spdlog::error("{}: Failed to get depth from caps", __func__);
			return GST_PAD_PROBE_REMOVE;
		}
		auto caps_str = gst_caps_to_string(caps);
		spdlog::info("{}: caps: {}", __func__, caps_str);
		g_free(caps_str);
		GstVideoInfo src_video_info;
		if(!gst_video_info_from_caps(&src_video_info, caps)) {
			spdlog::error("{}: Failed to get video info from caps", __func__);
			return GST_PAD_PROBE_REMOVE;
		}
		guint formats_len;
		auto format_iter = gst_video_formats_any(&formats_len);

		std::unique_ptr<Caps> out_caps = nullptr;

		// iterate over every possible video format to check if it has the same bit-depth and does not subsample
		for(guint format_idx = 0; format_idx < formats_len; format_idx++) {
			auto out_video_info = gst_video_format_get_info(format_iter[format_idx]);
			bool depth_match = false;
			bool has_subsampling = false;
			// ALL components must match our depth
			for(guint i = 0; i < out_video_info->n_components; i++) {
				if(out_video_info->h_sub[i] > 1 || out_video_info->h_sub[i] > 1) {
					spdlog::debug("{}: rejected. format {} has subsampling", __func__, out_video_info->name);
					has_subsampling = true;
					break;
				}
				if(out_video_info->depth[i] == static_cast<guint>(depth)) {
					depth_match = true;
				} else {
					spdlog::debug("{}: rejected. format {} has depth {}", __func__, out_video_info->name,
					              out_video_info->depth[i]);
					depth_match = false;
					break;
				}
			}
			// only allow conversion to formats that exactly match our bit-depth and do not subsample any channel
			if(!depth_match || has_subsampling) {
				continue;
			}
			spdlog::debug("{}: found format match: {}", __func__, out_video_info->name);

			Keela::Caps tmp_caps = Keela::Caps();
			tmp_caps.set_format(out_video_info->name);
			if(out_caps) {
				gst_caps_append(*out_caps, tmp_caps);
			} else {
				out_caps = std::make_unique<Keela::Caps>(tmp_caps);
			}
		}

		if(out_caps) {
			spdlog::info("{}: setting caps filter caps", __func__);
			auto *rb = static_cast<RecordBin *>(user_data);
			g_object_set(rb->caps_filter, "caps", static_cast<GstCaps *>(*out_caps), nullptr);
		}
		return GST_PAD_PROBE_REMOVE;
	}
	return GST_PAD_PROBE_PASS;
}

Keela::RecordBin::RecordBin() : QueueBin() {
	spdlog::info("{}", __func__);
	RecordBin::init();
	RecordBin::link();
}

void Keela::RecordBin::set_directory(const std::string &full_filename) {
	// try not to call this method once recording has started
	g_object_set(sink, "location", full_filename.c_str(), nullptr);
}

Keela::RecordBin::~RecordBin() {
	spdlog::debug(__func__);
}
