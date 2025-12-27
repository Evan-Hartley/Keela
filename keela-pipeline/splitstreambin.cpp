//
// Created by brand on 12/26/2025.
//

#include "keela-pipeline/splitstreambin.h"

#include "keela-pipeline/utils.h"
Keela::SplitStreamBin::SplitStreamBin() : even_stream("stream_even"), odd_stream("stream_odd") {
}
void Keela::SplitStreamBin::init() {
	add_elements(tee, even_stream, odd_stream);
}
void Keela::SplitStreamBin::link() {
	element_link_many(tee, even_stream);
	element_link_many(tee, odd_stream);
	link_queue(even_stream);
}

GstPadProbeReturn Keela::SplitStreamBin::frame_parity_probe_cb(GstPad *pad, GstPadProbeInfo *info, gpointer user_data) {
	GstBuffer *buffer = GST_PAD_PROBE_INFO_BUFFER(info);
	FrameProbeData *probe_data = static_cast<FrameProbeData *>(user_data);
	int parity = probe_data->parity;

	int frame_number = 0;

	// Some sources will have the frame number in the buffer offset (e.g.
	// videotestsrc)
	if(GST_BUFFER_OFFSET(buffer) != GST_BUFFER_OFFSET_NONE) {
		frame_number = GST_BUFFER_OFFSET(buffer);
	}
	// But others like aravissrc do not set offset, so we fall back to our own
	// per-camera counter
	else {
		// TODO: resolve narrowing conversion here
		frame_number = (*probe_data->counter)++;
	}

	if(frame_number % 2 == parity) {
		return GST_PAD_PROBE_OK;  // Pass the frame
	} else {
		return GST_PAD_PROBE_DROP;  // Drop the frame
	}
}