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

	void set_state(GstState state, bool wait = true);

   public:
	Keela::Element *get_parent() const;

   protected:
	/// associate a higher-level Keela::Element as the parent of this element. *parent __MUST__ be the same object
	/// associated with the GstElement*'s parent
	void set_parent(Keela::Element *parent);
	friend class Bin;

   private:
	Keela::Element *m_parent = nullptr;
};
}  // namespace Keela
#endif  // ELEMENTBASE_H
