//
// Created by brand on 5/26/2025.
//

#ifndef BIN_H
#define BIN_H
#include <gstreamer-1.0/gst/gst.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <stdexcept>
#include <string>

#include "elementbase.h"

namespace Keela {
class Bin : public virtual Keela::Element {
   public:
	explicit Bin(const std::string &name);

	// Bin(const Bin &bin);
	Bin();

	~Bin() override;

	operator GstElement *() const override;

	operator GstBin *() const;

	/// dump the bin data to a graphviz graph
	void dump_bin_graph() const;

	/**
	 * use this function to add many elements to the bin. If we use `gst_bin_add_many` directly,
	 * temporaries will be created using the implicit copy constructor which will mess up the underlying
	 * `GstElement` refcount.
	 * @tparam First any type which can convert to GstElement*
	 */
	template <typename First, typename... Rest>
	void add_elements(First first, Rest... rest) {
		GstElement *e = first;

		gst_object_ref(e);
		GstElement *b = *this;
		auto ret = gst_bin_add(GST_BIN(b), GST_ELEMENT(e));
		if(!ret) {
			throw std::runtime_error("Failed to add element to bin");
		}
		add_elements(rest...);
	}

	static void add_elements() {
		spdlog::info("{} Added all elements to bin", __func__);
	};

   protected:
	std::shared_ptr<GstBin> bin;
	// GstBin *bin;

	/**
	 * Create a ghost pad for an internal GstElement
	 */
	void add_ghost_pad(GstElement *element, const std::string &pad_name) const;

   private:
	/* Create required elements and add them to the bin
	 */
	virtual void init() {};

	/* Link elements together
	 */
	virtual void link() {
	}
};
}  // namespace Keela
#endif  // BIN_H
