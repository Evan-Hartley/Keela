//
// Created by brand on 5/26/2025.
//

#include "keela-pipeline/transformbin.h"

#include <spdlog/spdlog.h>

#include <stdexcept>

#include "keela-pipeline/gst-helpers.h"
#include "keela-pipeline/utils.h"

Keela::TransformBin::TransformBin() : QueueBin() {
	spdlog::info("{}", __func__);
	TransformBin::init();
	TransformBin::link();
}

Keela::TransformBin::TransformBin(const std::string &name) : QueueBin(name) {
	spdlog::info("{}", __func__);
	TransformBin::init();
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
	} catch(const std::exception &e) {
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
	} catch(const std::exception &e) {
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

void Keela::TransformBin::rotate(const std::string &direction) const {
	try {
		const auto variant =
		    gst_enum_variant_by_nick(G_OBJECT(static_cast<GstElement *>(rotation)), FLIP_PROP, direction);

		g_object_set(rotation, FLIP_PROP.c_str(), variant, nullptr);
	} catch(const std::exception &e) {
		spdlog::error("Could not set rotation of transformbin: {}", e.what());
		throw;
	}
}

void Keela::TransformBin::init() {
	add_elements(rotation, flip_h, flip_v, left_shift);
}

void Keela::TransformBin::link() {
	element_link_many(left_shift, rotation, flip_h, flip_v);
	link_queue(left_shift);

	add_ghost_pad(flip_v, "src");
}
