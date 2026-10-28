//
// Created by brand on 1/5/2026.
//

#ifndef LEFTSHIFTBIN_H
#define LEFTSHIFTBIN_H
#include "bin.h"
#include "caps.h"
#include "simpleelement.h"

namespace Keela {
/**
 * A bin that will left-shift packed bits from the LSB position to the MSB position, if necessary
 */
class LeftShiftBin : public Keela::Bin {
   public:
	LeftShiftBin();
	~LeftShiftBin() override;

	void set_enabled(bool enabled);

   private:
	void init() override;
	void link() override;

	std::shared_ptr<Caps> caps = nullptr;
	/// controls whether to perform left shift operation
	bool left_shift = true;
	SimpleElement caps_filter = SimpleElement("capsfilter");
	SimpleElement identity = SimpleElement("identity");
	gulong data_probe_id = 0;
	static GstPadProbeReturn data_probe(GstPad *pad, GstPadProbeInfo *info, gpointer user_data);
	static GstPadProbeReturn caps_probe(GstPad *pad, GstPadProbeInfo *info, gpointer user_data);
};

}  // namespace Keela

#endif  // LEFTSHIFTBIN_H
