//
// Created by brand on 6/13/25.
//
#include <gtest/gtest.h>
#include <keela-widgets/GLCameraRender.h>

#include "keela-pipeline/consts.h"
#include "keela-widgets/cameramanager.h"

/*
TEST(KeelaWidgets, ConstructGLCameraRender) {
    auto bin = std::make_shared<Keela::PresentationBin>();
    auto c = Keela::GLCameraRender(bin);
}*/

TEST(KeelaWidgets, ConstructCameraManager) {
	auto cm = Keela::CameraManager(0, false);
}

TEST(KeelaWidgets, CameraManagerStreamSplit) {
	auto cm = Keela::CameraManager(0, false);
	gst_element_set_state(cm, GST_STATE_PLAYING);
	ASSERT_FALSE(cm.is_frame_splitting_enabled());
	cm.set_frame_splitting(true);
	ASSERT_TRUE(cm.is_frame_splitting_enabled());
	gst_element_set_state(cm, GST_STATE_NULL);
}

TEST(KeelaWidgets, CameraManagerGetStreams) {
	auto cm = Keela::CameraManager(0, false);
	gst_element_set_state(cm, GST_STATE_PLAYING);
	ASSERT_EQ(cm.get_streams().size(), 1);
	cm.set_frame_splitting(true);
	ASSERT_EQ(cm.get_streams().size(), 2);
	gst_element_set_state(cm, GST_STATE_NULL);
}