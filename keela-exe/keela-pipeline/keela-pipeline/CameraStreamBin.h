#ifndef CAMERASTREAMBIN_H
#define CAMERASTREAMBIN_H

#include <keela-pipeline/presentationbin.h>
#include <keela-pipeline/recordbin.h>
#include <keela-pipeline/snapshotbin.h>

#include "EjectableElement.h"
#include "caps.h"
#include "keela-pipeline/TraceBin.h"
#include "queuebin.h"
#include "simpleelement.h"

namespace Keela {
class CameraStreamBin final : public QueueBin, public EjectableElement {
   public:
	explicit CameraStreamBin(const std::string &name);

	~CameraStreamBin() override;

	SimpleElement internal_tee = SimpleElement("tee");

	std::shared_ptr<PresentationBin> presentation;
	std::shared_ptr<SnapshotBin> snapshot;
	std::shared_ptr<TraceBin> trace;

	std::shared_ptr<RecordBin> record_bin = nullptr;

	void start_recording(const std::string &filename);

	void stop_recording();

	std::shared_ptr<TraceBin> get_trace_bin() {
		return trace;
	}

   private:
	void link() override;

	Keela::Element *Head() override {
		return &queue;
	};

	std::vector<Keela::Element *> Leaves() override {
		if(record_bin != nullptr) {
			return std::vector<Keela::Element *>{record_bin.get()};
		}
		return std::vector<Keela::Element *>{};
	}

	std::string name;
};
}  // namespace Keela
#endif  // CAMERASTREAMBIN_H
