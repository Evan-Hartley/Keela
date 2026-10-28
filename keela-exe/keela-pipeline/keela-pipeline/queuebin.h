//
// Created by brand on 5/28/2025.
//

#ifndef QUEUEBIN_H
#define QUEUEBIN_H
#include <keela-pipeline/bin.h>

#include "EjectableElement.h"
#include "simpleelement.h"

namespace Keela {
class QueueBin : public Bin {
   public:
	QueueBin();

	~QueueBin() override;

	explicit QueueBin(const std::string &name);

	Keela::SimpleElement queue = SimpleElement("queue");

   protected:
	template <typename T>
	void link_queue(T t) const {
		auto sink = to_gst_element(t);
		auto name = gst_element_get_name(sink);
		spdlog::info("Linking internal queue to {}", name);
		g_free(name);
		if(!gst_element_link(queue, sink)) {
			throw std::runtime_error("Failed to link queue");
		}
	}

   private:
	void init() override;

	void link() override;
};
}  // namespace Keela
#endif  // QUEUEBIN_H
