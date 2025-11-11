//
// Created by brand on 5/31/2025.
//

#include "keela-widgets/labeledcomboboxtext.h"

Keela::LabeledComboBoxText::LabeledComboBoxText(const std::string &name) {
	set_spacing(10);
	m_label.set_label(name);

	LabeledComboBoxText::add(m_combo);
	LabeledComboBoxText::add(m_label);
}
