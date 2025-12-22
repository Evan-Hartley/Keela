//
// Created by brand on 12/22/2025.
//

#ifndef IRESTARTABLE_H
#define IRESTARTABLE_H
#include <sigc++/signal.h>
#include <spdlog/spdlog.h>

#include <vector>
namespace Keela {
/// any object which needs to perform some action upon a pipeline restart
class IRestartable {
   public:
	virtual ~IRestartable() = default;

	virtual auto restart() -> void {
		spdlog::info("IRestartable::{} restarting all children", __func__);
		signal_restart.emit();
	}

	sigc::signal<void> signal_restart;
};
}  // namespace Keela
#endif  // IRESTARTABLE_H
