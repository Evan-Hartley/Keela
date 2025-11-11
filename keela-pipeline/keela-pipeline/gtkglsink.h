//
// Created by brand on 5/31/2025.
//

#ifndef GTKGLSINK_H
#define GTKGLSINK_H
#include "bin.h"
#include "simpleelement.h"
#include "widgetelement.h"

namespace Keela {
/// Creates an OpenGL based GtkWidget to display video
class GtkGlSink final : public Bin, public WidgetElement {
   public:
	GtkGlSink();

	explicit GtkGlSink(const std::string &name);

	~GtkGlSink() override;

   private:
	void init() override;

	void link() override;

   public:
	gpointer get_widget() override;

   private:
	Keela::SimpleElement glsink = SimpleElement("glsink");
	Keela::SimpleElement gtkglsink = SimpleElement("gtkglsink");
	// GstElement* glsink = nullptr;
	// GstElement* gtkglsink = nullptr;
};
}  // namespace Keela
#endif  // GTKGLSINK_H
