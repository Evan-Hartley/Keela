//
// Created by brand on 5/26/2025.
//

#ifndef LABELEDSPINBUTTON_H
#define LABELEDSPINBUTTON_H
#include <gtkmm-3.0/gtkmm.h>
namespace Keela {

/**
 * Composition of Gtk::Label and Gtk::SpinButton
 */
class LabeledSpinButton final : public Gtk::Box {
   public:
	/**
	 * @param label label text of spin button
	 */
	explicit LabeledSpinButton(const std::string& label);
	LabeledSpinButton() = default;
	Gtk::Label m_label;
	Gtk::SpinButton m_spin;
};
}  // namespace Keela
#endif  // LABELEDSPINBUTTON_H
