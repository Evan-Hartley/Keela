//
// Created by brand on 5/30/2025.
//

#include "./keela-pipeline/simpleelement.h"

#include <spdlog/spdlog.h>

#include <stdexcept>
#include <utility>

#include "keela-pipeline/utils.h"

Keela::SimpleElement::SimpleElement(const std::string &element) {
	element_name = element;
	spdlog::info("{} {}", __func__, element);
	auto e = gst_element_factory_make(element.c_str(), nullptr);
	if(!e) {
		throw std::runtime_error("Failed to create element: " + element);
	}

	m_element = std::shared_ptr<GstElement>(e, Keela::delete_element);
}

Keela::SimpleElement::~SimpleElement() {
	spdlog::debug("{} {}", __func__, element_name);
}

Keela::SimpleElement::SimpleElement(const std::string &element, const std::string &name) : SimpleElement(element) {
	spdlog::info("Named {} {}", __func__, name);
	const auto ret = gst_element_set_name(m_element.get(), name.c_str());
	if(!ret) {
		throw std::runtime_error("Failed to set name of element");
	}
}

Keela::SimpleElement::operator struct _GstElement *() const {
	if(m_element) {
		const gchar *tname = g_type_name(G_OBJECT_TYPE(m_element.get()));
		const gchar *name = GST_ELEMENT_NAME(m_element.get());
		if(!name) {
			spdlog::trace("{} Type name {}", __func__, tname);
		} else {
			spdlog::trace("{}::{} Type name {}", name, __func__, tname);
		}
		return GST_ELEMENT(m_element.get());
	}
	const std::string message = "Internal element is not initialized";
	spdlog::error(message);
	throw std::runtime_error(message);
}
