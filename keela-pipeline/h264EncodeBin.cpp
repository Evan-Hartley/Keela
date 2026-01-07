//
// Created by brand on 1/7/26.
//

#include "keela-pipeline/h264EncodeBin.h"

#include <gst/video/video-info.h>

#include "keela-pipeline/caps.h"
#include "keela-pipeline/utils.h"

Keela::H264EncodeBin::H264EncodeBin(): Keela::Bin("H264EncodeBin") {
}

void Keela::H264EncodeBin::init() {
    g_object_set(enc,"quantizer",0,nullptr);
    g_object_set(enc,"pass","quant",nullptr);
    add_elements(video_convert,caps_filter,enc);
	auto pad = gst_element_get_static_pad(video_convert, "src");
	gst_pad_add_probe(pad, static_cast<GstPadProbeType>(GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM | GST_PAD_PROBE_TYPE_BLOCK),
					  caps_probe, this, nullptr);
	gst_object_unref(pad);
}

void Keela::H264EncodeBin::link() {
    element_link_many(video_convert,caps_filter,enc);
    add_ghost_pad(video_convert,"sink");
    add_ghost_pad(enc,"src");
}

GstPadProbeReturn Keela::H264EncodeBin::caps_probe(GstPad *pad, GstPadProbeInfo *info, void *user_data) {
    	if(!(GST_PAD_PROBE_INFO_TYPE(info) & GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM))
		return GST_PAD_PROBE_PASS;
	auto event = gst_pad_probe_info_get_event(info);
	if(GST_EVENT_TYPE(event) == GST_EVENT_CAPS) {
		SPDLOG_INFO("{}: got caps event", __func__);
		/// wrap raw caps pointer into Keela::Caps
		std::unique_ptr<Keela::Caps> caps = nullptr;
		{
			GstCaps *tmp;
			gst_event_parse_caps(event, &tmp);
			caps = std::make_unique<Keela::Caps>(tmp);
		}
		auto structure = gst_caps_get_structure(*caps, 0);
		gint depth;
		if(!gst_structure_get_int(structure, "depth", &depth)) {
			SPDLOG_ERROR("{}: Failed to get depth from caps", __func__);
			return GST_PAD_PROBE_REMOVE;
		}
		auto caps_str = gst_caps_to_string(*caps);
		spdlog::info("{}: caps: {}", __func__, caps_str);
		g_free(caps_str);
		GstVideoInfo src_video_info;
		if(!gst_video_info_from_caps(&src_video_info, *caps)) {
			SPDLOG_ERROR("{}: Failed to get video info from caps", __func__);
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
				if(out_video_info->h_sub[i] > 1 || out_video_info->w_sub[i] > 1) {
					SPDLOG_DEBUG("{}: rejected. format {} has subsampling", __func__, out_video_info->name);
					has_subsampling = true;
					break;
				}
				if(out_video_info->depth[i] == static_cast<guint>(depth)) {
					depth_match = true;
				} else {
					SPDLOG_DEBUG("{}: rejected. format {} has depth {}", __func__, out_video_info->name,
					             out_video_info->depth[i]);
					depth_match = false;
					break;
				}
			}
			// only allow conversion to formats that exactly match our bit-depth and do not subsample any channel
			if(!depth_match || has_subsampling) {
				continue;
			}
			SPDLOG_DEBUG("{}: found format match: {}", __func__, out_video_info->name);

			Keela::Caps tmp_caps = Keela::Caps();
			tmp_caps.set_format(out_video_info->name);
			if(out_caps) {
				gst_caps_append(*out_caps, tmp_caps);
			} else {
				out_caps = std::make_unique<Keela::Caps>(tmp_caps);
			}
		}

		if(out_caps) {
			SPDLOG_INFO("{}: setting caps filter caps", __func__);
			auto self = static_cast<H264EncodeBin *>(user_data);
			g_object_set(self->caps_filter, "caps", static_cast<GstCaps *>(*out_caps), nullptr);
		}
		return GST_PAD_PROBE_REMOVE;
	}
	return GST_PAD_PROBE_PASS;
}
