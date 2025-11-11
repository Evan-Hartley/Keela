//
// Created by brand on 5/30/2025.
//

#ifndef ELEMENT_H
#define ELEMENT_H
#include <gstreamer-1.0/gst/gst.h>

#include <memory>
#include <string>

#include "elementbase.h"

namespace Keela {
class SimpleElement : public virtual Keela::Element {
   public:
	explicit SimpleElement(const std::string &element);

	~SimpleElement() override;

	SimpleElement(const std::string &element, const std::string &name);

	operator GstElement *() const override;

   protected:
	std::string element_name;
	std::shared_ptr<GstElement> m_element;
};
}  // namespace Keela
#endif  // ELEMENT_H
