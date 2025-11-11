//
// Created by brand on 5/31/2025.
//

#ifndef LABELEDCOMBOBOXTEXT_H
#define LABELEDCOMBOBOXTEXT_H
#include <gtkmm-3.0/gtkmm.h>

namespace Keela {
class LabeledComboBoxText final : public Gtk::Box {
   public:
	explicit LabeledComboBoxText(const std::string &name);

	Gtk::ComboBoxText m_combo;

   private:
	Gtk::Label m_label;
};
}  // namespace Keela
#endif  // LABELEDCOMBOBOXTEXT_H
