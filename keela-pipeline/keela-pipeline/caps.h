//
// Created by brand on 6/7/2025.
//

#ifndef CAPS_H
#define CAPS_H
#include <gstreamer-1.0/gst/gst.h>

#include <memory>
#include <stdexcept>

namespace Keela {
class Caps {
   public:
	Caps();

	explicit Caps(GstCaps *c);

	~Caps();

	operator GstCaps *() const;

	void set_framerate(int numerator, int denominator);

	void set_resolution(int width, int height);

   private:
	std::shared_ptr<GstCaps> m_caps;

	template <typename... Args>
	void set_props(Args... args) {
		if(!gst_caps_is_writable(m_caps.get())) {
			throw std::runtime_error("Caps is not writable");
		}
		gst_caps_set_simple(m_caps.get(), args..., nullptr);
	}
};
}  // namespace Keela
#endif  // CAPS_H
