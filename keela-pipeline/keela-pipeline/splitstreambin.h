//
// Created by brand on 12/26/2025.
//

#ifndef SPLITSTREAMBIN_H
#define SPLITSTREAMBIN_H
#include "CameraStreamBin.h"
#include "EjectableElement.h"
#include "queuebin.h"
namespace Keela {
class SplitStreamBin final : public EjectableElement, public QueueBin {
   public:
	SplitStreamBin();
	~SplitStreamBin() override = default;

   private:
	Keela::CameraStreamBin even_stream;
	Keela::CameraStreamBin odd_stream;
	Keela::SimpleElement tee = SimpleElement("tee");
	Keela::Element *Head() override;
	std::vector<Keela::Element *> Leaves() override;
	void init() override;
	void link() override;

	struct FrameProbeData {
		int parity;
		guint64 *counter;
	};

	enum FrameParity { EVEN, ODD };

	FrameProbeData even_probe_data = FrameProbeData(FrameParity::EVEN, &backup_counter);
	FrameProbeData odd_probe_data = FrameProbeData(FrameParity::ODD, &backup_counter);

	guint64 backup_counter = 0;

	/// frame filtering callback
	static GstPadProbeReturn frame_parity_probe_cb(GstPad *pad, GstPadProbeInfo *info, gpointer user_data);
};
}  // namespace Keela
#endif  // SPLITSTREAMBIN_H
