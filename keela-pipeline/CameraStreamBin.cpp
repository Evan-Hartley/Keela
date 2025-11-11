#include "keela-pipeline/CameraStreamBin.h"

#include <keela-pipeline/gtkglsink.h>
#include <keela-pipeline/gtksink.h>
#include <spdlog/spdlog.h>

#include <stdexcept>

#include "keela-pipeline/utils.h"

Keela::CameraStreamBin::CameraStreamBin(const std::string &name) : QueueBin(name), name(name) {
	spdlog::info("Creating CameraStreamBin: {}", name);
	presentation = std::make_shared<PresentationBin>("presentation_" + this->name);
	snapshot = std::make_shared<SnapshotBin>("snapshot_" + this->name);
	trace = std::make_shared<TraceBin>("trace_" + this->name);

	CameraStreamBin::link();
}

Keela::CameraStreamBin::~CameraStreamBin() {
	spdlog::debug(__func__);
}

void Keela::CameraStreamBin::link() {
	spdlog::info("Linking CameraStreamBin {} internal structure", name);

	// Add internal tee and all child bins to this bin
	add_elements(internal_tee, *presentation, *snapshot, *trace);

	// Link internal_tee -> all outputs
	element_link_many(internal_tee, *presentation);
	element_link_many(internal_tee, *snapshot);
	element_link_many(internal_tee, *trace);
	link_queue(internal_tee);
}

void Keela::CameraStreamBin::start_recording(const std::string &filename) {
	record_bin = std::make_shared<RecordBin>("recordbin_" + name);
	record_bin->set_directory(filename);
	add_elements(static_cast<Bin &>(*record_bin));
	gboolean sync_result = gst_element_sync_state_with_parent(static_cast<Bin &>(*record_bin));

	if(!sync_result) {
		throw std::runtime_error("Failed to sync record_bin state with parent");
	}

	element_link_many(internal_tee, static_cast<Bin &>(*record_bin));
	spdlog::info("Started recording in CameraStreamBin {} to file {}", name, filename);
}

void Keela::CameraStreamBin::stop_recording() {
	// notes from example to dynamically remove a bin from a playing pipeline:
	// add a blocking downstream probe to the queue "src" pad
	// inside the blocking callback, add the EOS probe to the last source pad of the bin (it is unclear if this can also
	// be a sink pad) after installing the EOS callback, send an EOS event to the sink pad of the beginning of the bin
	// inside the EOS callback, set the state of the bin to NULL and remove the bin from the pipeline
	record_bin->PrepareEject();
	record_bin->Eject(false);

	record_bin = nullptr;
	spdlog::info("Removed recordbin from {} pipeline", name);
}