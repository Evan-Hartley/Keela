#include <gtest/gtest.h>

#include "../keela-widgets/keela-widgets/cameramanager.h"
#include "keela-pipeline/consts.h"
//
// Created by brand on 6/3/2025.
//

TEST(KeelaExe, ConstructCameraManager) {
	Keela::CameraManager cm(1, GRAY8, false);
	GstElement *b = cm;
	gst_debug_bin_to_dot_file(GST_BIN(b), GST_DEBUG_GRAPH_SHOW_ALL, "cameramanager1");
}

TEST(KeelaExe, PlayCameraManager) {
	auto pipeline = gst_pipeline_new("pipeline");
	Keela::CameraManager *camera = new Keela::CameraManager(1, GRAY8, false);
	guint *refcount = &GST_OBJECT_REFCOUNT(static_cast<GstElement *>(*camera));
	EXPECT_EQ(*refcount, 1);
	spdlog::info("adding cameramanager to pipeline");
	gst_bin_add(GST_BIN(pipeline), *camera);
	EXPECT_EQ(*refcount, 1);
	spdlog::info("removing cameramanager from pipeline");
	gst_bin_remove(GST_BIN(pipeline), *camera);
	EXPECT_FALSE(GST_IS_ELEMENT(camera));
	spdlog::info("deleting cameramanager");
	delete camera;
	g_object_unref(pipeline);
}
