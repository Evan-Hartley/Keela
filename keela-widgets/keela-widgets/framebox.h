//
// Created by brand on 5/26/2025.
//

#ifndef FRAMEBOX_H
#define FRAMEBOX_H
#include <gtkmm-3.0/gtkmm.h>
namespace Keela {
class FrameBox final : public Gtk::Frame {
   public:
	FrameBox();
	explicit FrameBox(const std::string& title, Gtk::Orientation orientation = Gtk::ORIENTATION_HORIZONTAL);
	void add(Widget& widget) override;
	void set_spacing(int i);

   private:
	Gtk::Box m_box;
};
}  // namespace Keela
#endif  // FRAMEBOX_H
