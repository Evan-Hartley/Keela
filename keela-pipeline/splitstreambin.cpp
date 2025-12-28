//
// Created by brand on 12/26/2025.
//

#include "keela-pipeline/splitstreambin.h"

#include "keela-pipeline/utils.h"
Keela::SplitStreamBin::SplitStreamBin() : QueueBin("SplitStreamBin") {
	even_stream = std::make_shared<Keela::CameraStreamBin>("stream_even");
	odd_stream = std::make_shared<Keela::CameraStreamBin>("stream_odd");
	SplitStreamBin::init();
	SplitStreamBin::link();
}
Keela::Element *Keela::SplitStreamBin::Head() {
	return &tee;
}
std::vector<Keela::Element *> Keela::SplitStreamBin::Leaves() {
	auto leaves = EjectableElement::GetLeaves(*even_stream);
	auto odd_leaves = EjectableElement::GetLeaves(*odd_stream);
	leaves.insert(leaves.end(), odd_leaves.begin(), odd_leaves.end());
	return leaves;
}
void Keela::SplitStreamBin::init() {
	add_elements(tee, *even_stream, *odd_stream);
	GstPad *p;
	p = gst_element_get_static_pad(even_stream->internal_tee, "sink");
	if(!p) {
		throw std::runtime_error("Failed to get even stream sink pad");
	}
	gst_pad_add_probe(p, GST_PAD_PROBE_TYPE_BUFFER, frame_parity_probe_cb, &even_probe_data, nullptr);
	g_object_unref(p);
	p = gst_element_get_static_pad(odd_stream->internal_tee, "sink");
	if(!p) {
		throw std::runtime_error("Failed to get odd stream sink pad");
	}
	gst_pad_add_probe(p, GST_PAD_PROBE_TYPE_BUFFER, frame_parity_probe_cb, &odd_probe_data, nullptr);
}
void Keela::SplitStreamBin::link() {
	element_link_many(tee, *even_stream);
	element_link_many(tee, *odd_stream);
	link_queue(tee);
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