//
// Created by brand on 12/10/2025.
//

#ifndef PIPELINE_H
#define PIPELINE_H
#include <memory>

#include "elementbase.h"
namespace Keela {
class Pipeline final : public Keela::Element {
   public:
	Pipeline();
	~Pipeline() override = default;
	operator struct _GstElement *() const override;

   private:
	std::shared_ptr<GstElement> m_pipeline;
};
}  // namespace Keela
#endif  // PIPELINE_H
