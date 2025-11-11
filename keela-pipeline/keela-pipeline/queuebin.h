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
	void link_queue(GstElement *sink) const;

   private:
	void init() override;

	void link() override;
};
}  // namespace Keela
#endif  // QUEUEBIN_H
