//
// Created by brand on 1/11/2026.
//

#include <gtest/gtest.h>
#include <keela-pipeline/FFV1EncodeBin.h>
#include <keela-pipeline/bin.h>
#include <keela-pipeline/caps.h>
#include <keela-pipeline/h264EncodeBin.h>
#include <keela-pipeline/simpleelement.h>
#include <keela-pipeline/videoencoder.h>
template <typename T>
class VideoEncoderTest : public ::testing::Test {
   protected:
	void SetUp() override {
		pipeline = gst_pipeline_new("VideoEncoderTest");
		encoder = std::make_shared<T>();
		bin.add_elements(testsrc, capsfilter, encoder, fakesink);
		Keela::element_link_many(testsrc, capsfilter, encoder, fakesink);
		gst_bin_add(GST_BIN(pipeline), bin);
	}
	void TearDown() override {
		bin.dump_bin_graph();
		gst_object_unref(pipeline);
	}

	GstElement *pipeline;
	Keela::Bin bin = Keela::Bin();
	Keela::SimpleElement testsrc = Keela::SimpleElement("videotestsrc");
	Keela::SimpleElement capsfilter = Keela::SimpleElement("capsfilter");
	std::shared_ptr<Keela::VideoEncoder> encoder;
	Keela::SimpleElement fakesink = Keela::SimpleElement("fakesink");
};

using EncoderTypes = ::testing::Types<Keela::H264EncodeBin, Keela::FFV1EncodeBin>;
TYPED_TEST_SUITE(VideoEncoderTest, EncoderTypes);

TYPED_TEST(VideoEncoderTest, Can_Negotiate_Caps_GRAY8) {
	auto caps = Keela::Caps();
	caps.set_format("GRAY8");
	g_object_set(this->capsfilter, "caps", static_cast<GstCaps *>(caps), nullptr);
	gst_element_set_state(this->pipeline, GST_STATE_PLAYING);
	GstState state;
	gst_element_get_state(this->pipeline, &state, nullptr, GST_CLOCK_TIME_NONE);
	ASSERT_EQ(state, GST_STATE_PLAYING);
}

TYPED_TEST(VideoEncoderTest, Can_Negotiate_Caps_GRAY16_LE) {
	auto caps = Keela::Caps();
	caps.set_format("GRAY16_LE");
	g_object_set(this->capsfilter, "caps", static_cast<GstCaps *>(caps), nullptr);
	gst_element_set_state(this->pipeline, GST_STATE_PLAYING);
	GstState state;
	gst_element_get_state(this->pipeline, &state, nullptr, GST_CLOCK_TIME_NONE);
	ASSERT_EQ(state, GST_STATE_PLAYING);
}

TYPED_TEST(VideoEncoderTest, Can_Negotiate_Caps_GRAY16_BE) {
	auto caps = Keela::Caps();
	caps.set_format("GRAY16_BE");
	g_object_set(this->capsfilter, "caps", static_cast<GstCaps *>(caps), nullptr);
	gst_element_set_state(this->pipeline, GST_STATE_PLAYING);
	GstState state;
	gst_element_get_state(this->pipeline, &state, nullptr, GST_CLOCK_TIME_NONE);
	ASSERT_EQ(state, GST_STATE_PLAYING);
}

/// this non-functional test ensures that the above functional tests are implemented correctly
TYPED_TEST(VideoEncoderTest, Can_Negotiate_Caps_break) {
	auto caps = Keela::Caps();
	caps.set_format("FictionalPixelFormat");
	g_object_set(this->capsfilter, "caps", static_cast<GstCaps *>(caps), nullptr);
	gst_element_set_state(this->pipeline, GST_STATE_PLAYING);
	GstState state;
	gst_element_get_state(this->pipeline, &state, nullptr, GST_CLOCK_TIME_NONE);
	ASSERT_NE(state, GST_STATE_PLAYING);
}