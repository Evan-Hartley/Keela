//
// Created by brand on 12/10/2025.
//

#ifndef PIPELINE_H
#define PIPELINE_H
#include <memory>

#include "bin.h"
#include "elementbase.h"
namespace Keela {
class Pipeline final : public Keela::Bin {
   public:
	Pipeline();
	~Pipeline() override = default;
};
}  // namespace Keela
#endif  // PIPELINE_H
