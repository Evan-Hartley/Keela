//
// Created by brand on 12/22/2025.
//

#ifndef IRESTARTABLE_H
#define IRESTARTABLE_H
#include <spdlog/spdlog.h>

#include <vector>
namespace Keela {
/// any object which needs to perform some action upon a pipeline restart
class IRestartable {
   public:
	virtual ~IRestartable() = default;

	virtual void restart() {
		spdlog::info("IRestartable::{} restarting all children", __func__);
		for(auto restartable : restartable_children()) {
			restartable.restart();
		}
	}

   protected:
	/// any sub-objects which also need to be restarted
	virtual std::vector<IRestartable> restartable_children() {
		return std::vector<IRestartable>();
	};
};
}  // namespace Keela
#endif  // IRESTARTABLE_H
