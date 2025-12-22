//
// Created by brand on 12/22/2025.
//

#ifndef IRECORDABLE_H
#define IRECORDABLE_H
#include <spdlog/spdlog.h>

#include <vector>
namespace Keela {
/// any object which needs to perform some action upon starting or stopping recording
/// TODO: need better name?
class IRecordable {
   public:
	virtual ~IRecordable() {
	}

	virtual void start_recording() {
		spdlog::info("Starting recording...");
		for(auto child : recordable_children()) {
			child.start_recording();
		}
	}

	virtual void stop_recording() {
		spdlog::info("Stopping recording...");
		for(auto child : recordable_children()) {
			child.stop_recording();
		}
	}

   private:
	virtual std::vector<IRecordable> recordable_children() {
		return std::vector<IRecordable>();
	}
};
}  // namespace Keela
#endif  // IRECORDABLE_H
