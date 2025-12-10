//
// Created by brand on 5/31/2025.
//

#include "keela-pipeline/elementbase.h"

#include <spdlog/spdlog.h>

#include <stdexcept>
void Keela::Element::set_state(GstState state, bool wait) {
	auto ret = gst_element_set_state(*this, state);
	auto state_name = gst_element_state_get_name(state);
	auto element_name = GST_ELEMENT_NAME(static_cast<GstElement *>(*this));
	spdlog::info("{} Change state to {}", element_name, state_name);
	switch(ret) {
		case GST_STATE_CHANGE_FAILURE:
			spdlog::error("{} Failed to change state to {}", element_name, state_name);
			throw std::runtime_error("Failed to set state of element");
		case GST_STATE_CHANGE_ASYNC:
			spdlog::warn("{} state change is async", element_name);
			if(wait) {
				ret = gst_element_get_state(*this, nullptr, nullptr, GST_CLOCK_TIME_NONE);
				if(ret != GST_STATE_CHANGE_FAILURE) {
					spdlog::error("{} async state change to {} failed", element_name, state_name);
					throw std::runtime_error("Async state change failed");
				}
			}
			break;
		default:
			spdlog::info("{} state changed successfully to {}", element_name, state_name);
	}
}