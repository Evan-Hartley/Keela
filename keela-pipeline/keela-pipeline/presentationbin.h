//
// Created by brand on 5/26/2025.
//

#ifndef PRESENTATIONBIN_H
#define PRESENTATIONBIN_H
#include <gstreamer-1.0/gst/gst.h>

#include "caps.h"
#include "queuebin.h"
#include "simpleelement.h"

namespace Keela {
class PresentationBin final : public QueueBin {
   public:
	explicit PresentationBin(const std::string &name);

	PresentationBin();

	~PresentationBin() override;

	void set_presentation_framerate(guint framerate);

	// gpointer get_widget();

	Keela::SimpleElement sink = SimpleElement("appsink");

   private:
	void init() override;

	void link() override;

	/// Used to skip frames for the purposes of presentation
	Keela::SimpleElement video_rate = SimpleElement("videorate");
	/// Controls the target presentation framerate
	Keela::SimpleElement caps_filter = SimpleElement("capsfilter");
	Keela::Caps presentation_caps = Caps();
};
}  // namespace Keela
#endif  // PRESENTATIONBIN_H
