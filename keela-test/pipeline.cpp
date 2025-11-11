//
// Created by brand on 5/27/2025.
//
#include <gtest/gtest.h>
#include <keela-pipeline/presentationbin.h>
#include <keela-pipeline/recordbin.h>
#include <keela-pipeline/simpleelement.h>
#include <keela-pipeline/transformbin.h>
#include <spdlog/spdlog.h>

TEST(KeelaPipeline, ConstructBin) {
	auto bin = Keela::Bin();
}

TEST(KeelaPipeline, ConstructNamedBin) {
	auto bin = Keela::Bin("Foo");
}

TEST(KeelaPipeline, ConstructRecordBin) {
	auto bin = Keela::RecordBin();
}

TEST(KeelaPipeline, ConstructNamedRecordBin) {
	auto bin = Keela::RecordBin("Foo");
}

TEST(KeelaPipeline, ConstructPresentationBin) {
	auto bin = Keela::PresentationBin();
}

TEST(KeelaPipeline, ConstructNamedPresentationBin) {
	auto bin = Keela::PresentationBin("Foo");
}

TEST(KeelaPipeline, ConstructTransformBin) {
	auto bin = Keela::TransformBin();
}

TEST(KeelaPipeline, ConstructNamedTransformBin) {
	auto bin = Keela::TransformBin("Foo");
}

#pragma region demonstrably false tests
TEST(KeelaPipeline, UseBinAsGstBin) {
	Keela::Bin bin("bin");
	GstElement *b = bin;
	ASSERT_TRUE(GST_IS_BIN(b));
}

TEST(KeelaPipeline, UseBinAsGstElement) {
	Keela::Bin bin("bin");
	GstElement *b = bin;
	ASSERT_TRUE(GST_IS_ELEMENT(b));
}
#pragma endregion

TEST(KeelaPipeline, DuplicateNamedBins) {
	// WARN: this is only valid if the two bins are not added to the same parent bin
	// which at runtime I believe should never happen as each camera should be getting a unique name
	auto bin1 = Keela::Bin("Foo");
	auto bin2 = Keela::Bin("Foo");
}

TEST(KeelaPipeline, LinkBins) {
	auto bin1 = Keela::TransformBin();
	auto bin2 = Keela::RecordBin();
	ASSERT_TRUE(gst_element_link(bin1, bin2));
}

TEST(KeelaPipeline, CanPlay) {
	GstElement *b = gst_pipeline_new("Playbin");
	ASSERT_TRUE(b != nullptr);
	Keela::SimpleElement src("videotestsrc", "TestSrc");
	Keela::TransformBin transform("TransformBin");
	Keela::SimpleElement presentation("autovideosink", "Presentation");
	spdlog::info("Created all elements");

	GstElement *s = src;
	GstElement *t = transform;
	GstElement *p = presentation;
	g_object_set(s, "num-buffers", 0, nullptr);      // set this to anything other than zero to see the window
	gst_bin_add_many(GST_BIN(b), s, t, p, nullptr);  // which causes this C function to fail
	ASSERT_TRUE(gst_element_link_many(s, t, p, nullptr));
	// TODO: link elements and set pipeline to playing

	gst_debug_bin_to_dot_file(GST_BIN(b), GST_DEBUG_GRAPH_SHOW_ALL, "CanPlay.dot");
	GstBus *bus = gst_element_get_bus(GST_ELEMENT(b));
	ASSERT_TRUE(bus != nullptr);
	spdlog::info("Starting playback");
	GstStateChangeReturn ret = gst_element_set_state(GST_ELEMENT(b), GST_STATE_PLAYING);
	ASSERT_TRUE(ret != GST_STATE_CHANGE_FAILURE);
	GstMessage *msg = nullptr;
	while((msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_EOS))) {
		spdlog::info("Got EOS");
		gst_message_unref(msg);
		break;
	}
}

TEST(KeelaPipeline, CreateCaps) {
	auto caps = Keela::Caps();
	caps.set_framerate(5000, 10);
	caps.set_resolution(640, 480);
	// caps2 aliases the shared pointer in caps
	auto caps2 = Keela::Caps(caps);
}

TEST(KeelaPipeline, CopyCaps) {
	auto caps1 = Keela::Caps();
	caps1.set_framerate(5000, 10);
	caps1.set_resolution(640, 480);
	auto caps2 = Keela::Caps(static_cast<GstCaps *>(caps1));
	ASSERT_TRUE(gst_caps_is_equal(caps1, caps2));
}
