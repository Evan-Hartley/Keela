//
// Created by brand on 7/7/2025.
//

#ifndef TRACEBIN_H
#define TRACEBIN_H
#include "caps.h"
#include "queuebin.h"

namespace Keela {
class TraceBin final : public QueueBin {
   public:
	TraceBin();

	explicit TraceBin(const std::string &name);

	~TraceBin() override;

	/*
	 * Whether to enable tracing or not
	 */
	void enable_trace(bool do_enable);

	/*
	 * Set the target framerate for trace subsampling
	 */
	void set_trace_framerate(guint framerate);

   private:
	void init() override;

	void link() override;

   public:
	SimpleElement video_rate = SimpleElement("videorate");
	SimpleElement caps_filter = SimpleElement("capsfilter");
	SimpleElement sink = SimpleElement("appsink");

   private:
	Caps trace_caps = Caps();
};
}  // namespace Keela
#endif  // TRACEBIN_H
