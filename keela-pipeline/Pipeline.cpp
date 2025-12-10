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

	bin = std::shared_ptr<GstBin>(GST_BIN(e), Keela::delete_bin);
}