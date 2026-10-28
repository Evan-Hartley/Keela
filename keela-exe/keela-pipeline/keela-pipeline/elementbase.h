//
// Created by brand on 5/31/2025.
//

#ifndef ELEMENTBASE_H
#define ELEMENTBASE_H
#include <gstreamer-1.0/gst/gst.h>

namespace Keela {
/**
 * Base class to enable interchangable use of Keela::SimpleElement and Keela::Bin in a pipeline
 */
class Element {
   public:
	virtual ~Element() = default;

	virtual operator GstElement *() const = 0;
};
}  // namespace Keela
#endif  // ELEMENTBASE_H
