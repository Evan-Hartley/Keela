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

TEST(KeelaWidgets, ConstructCameraManagerGRAY8) {
	auto cm = Keela::CameraManager(0, GRAY8, false);
}

TEST(KeelaWidgets, ConstructCameraManagerGRAY16LE) {
	auto cm = Keela::CameraManager(0, GRAY16_LE, false);
}

TEST(KeelaWidgets, ConstructCameraManagerGRAY16BE) {
	auto cm = Keela::CameraManager(0, GRAY16_BE, false);
}
