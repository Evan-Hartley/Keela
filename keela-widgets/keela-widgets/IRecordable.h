//
// Created by brand on 12/22/2025.
//

#ifndef IRECORDABLE_H
#define IRECORDABLE_H
#include <sigc++/signal.h>
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
		signal_start_recording.emit();
	}

	virtual void stop_recording() {
		spdlog::info("Stopping recording...");
		signal_stop_recording.emit();
	}

	sigc::signal<void> signal_stop_recording;
	sigc::signal<void> signal_start_recording;
};
}  // namespace Keela
#endif  // IRECORDABLE_H
