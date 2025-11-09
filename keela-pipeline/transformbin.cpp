//
// Created by brand on 5/26/2025.
//

#include "keela-pipeline/transformbin.h"

#include <stdexcept>
#include <spdlog/spdlog.h>

#include "keela-pipeline/gst-helpers.h"
#include "keela-pipeline/utils.h"

Keela::TransformBin::TransformBin(): QueueBin() {
    spdlog::info("{}", __func__);
    TransformBin::init();
    TransformBin::link();
}

Keela::TransformBin::TransformBin(const std::string &name): QueueBin(name) {
    spdlog::info("{}", __func__);
    TransformBin::init();
    gboolean ret = false;
    ret = gst_object_set_name(GST_OBJECT(static_cast<GstElement*>(video_scale)), (name + "_scale").c_str());
    if (!ret) {
        spdlog::warn("{} Failed to name elements", __func__);
    }
    TransformBin::link();
}

Keela::TransformBin::~TransformBin() {
    spdlog::debug(__func__);
}

void Keela::TransformBin::flip_horizontal(const bool apply_flip) const {
    auto direction = apply_flip ? FLIP_HORIZONTAL : IDENTITY;
    try {
        const auto variant =
                gst_enum_variant_by_nick(G_OBJECT(static_cast<GstElement *>(flip_h)), FLIP_PROP, direction);

        g_object_set(flip_h, FLIP_PROP.c_str(), variant, nullptr);
    } catch (const std::exception &e) {
        spdlog::error("Could not set horizontal flip of transformbin: {}", e.what());
        throw;
    }
}

void Keela::TransformBin::flip_vertical(bool apply_flip) const {
    auto direction = apply_flip ? FLIP_VERTICAL : IDENTITY;
    try {
        const auto variant =
                gst_enum_variant_by_nick(G_OBJECT(static_cast<GstElement *>(flip_v)), FLIP_PROP, direction);

        g_object_set(flip_v, FLIP_PROP.c_str(), variant, nullptr);
    } catch (const std::exception &e) {
        spdlog::error("Could not set vertical flip of transformbin: {}", e.what());
        throw;
    }
}

void Keela::TransformBin::rotate_identity() const {
    rotate(IDENTITY);
}

void Keela::TransformBin::rotate_90() const {
    rotate(ROTATE_90);
}

void Keela::TransformBin::rotate_180() const {
    rotate(ROTATE_180);
}

void Keela::TransformBin::rotate_270() const {
    rotate(ROTATE_270);
}

void Keela::TransformBin::scale(const int width, const int height) {
    scaled_caps = Caps(static_cast<GstCaps *>(scaled_caps));
    const int w = width / 2;
    const int h = height / 2;
    scaled_caps.set_resolution(w, h);
    g_object_set(scaled_caps_filter, "caps", static_cast<GstCaps *>(scaled_caps), nullptr);
}

void Keela::TransformBin::rotate(const std::string &direction) const {
    try {
        const auto variant =
                gst_enum_variant_by_nick(G_OBJECT(static_cast<GstElement *>(rotation)), FLIP_PROP, direction);

        g_object_set(rotation, FLIP_PROP.c_str(), variant, nullptr);
    } catch (const std::exception &e) {
        spdlog::error("Could not set rotation of transformbin: {}", e.what());
        throw;
    }
}

void Keela::TransformBin::init() {
    auto variant = Keela::gst_enum_variant_by_nick(
        G_OBJECT(static_cast<GstElement*>(video_scale)), "method", "bilinear");
    g_object_set(video_scale, "method", variant, nullptr);
}

void Keela::TransformBin::link() {
    add_elements(native_caps_filter, video_scale, scaled_caps_filter, rotation, flip_h, flip_v);
    element_link_many(native_caps_filter, video_scale, scaled_caps_filter, rotation, flip_h, flip_v);
    link_queue(native_caps_filter);

    add_ghost_pad(flip_v, "src");
}

void Keela::TransformBin::SetResolution(uint32_t width, uint32_t height) {
    native_caps = Caps(static_cast<GstCaps *>(native_caps));
    scaled_caps = Caps(static_cast<GstCaps *>(scaled_caps));

    resolution_width = width;
    resolution_height = height;
    scaled_caps.set_resolution(width, height);
    auto native_w = width * binning_factor;
    auto native_h = height * binning_factor;
    native_caps.set_resolution(native_w, native_h);

    g_object_set(scaled_caps_filter, "caps", static_cast<GstCaps *>(scaled_caps), nullptr);
    g_object_set(native_caps_filter, "caps", static_cast<GstCaps *>(native_caps), nullptr);
}

void Keela::TransformBin::SetBinning(uint32_t binning) {
    binning_factor = binning;
    SetResolution(resolution_width, resolution_height);
}

void Keela::TransformBin::ResetCameraResolution() {
}
