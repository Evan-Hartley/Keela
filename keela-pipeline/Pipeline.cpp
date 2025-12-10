//
// Created by brand on 12/10/2025.
//

#include "keela-pipeline/Pipeline.h"

#include "keela-pipeline/utils.h"
Keela::Pipeline::Pipeline() {
	auto e = gst_pipeline_new(nullptr);
	if(!e) {
		throw std::runtime_error("Failed to create pipeline");
	}

	m_pipeline = std::shared_ptr<GstElement>(e, Keela::delete_element);
}
Keela::Pipeline::operator struct _GstElement *() const {
	if(m_pipeline) {
		return GST_ELEMENT(m_pipeline.get());
	}
	const std::string message = "Internal pipeline is not initialized";
	spdlog::error(message);
	throw std::runtime_error(message);
}