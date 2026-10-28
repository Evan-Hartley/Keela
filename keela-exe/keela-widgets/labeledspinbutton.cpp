//
// Created by brand on 5/26/2025.
//

#include "keela-widgets/labeledspinbutton.h"

Keela::LabeledSpinButton::LabeledSpinButton(const std::string& label) : Box() {
	m_label.set_label(label);
	m_label.set_mnemonic_widget(m_spin);
	set_spacing(10);
	set_border_width(5);
	Keela::LabeledSpinButton::add(m_spin);
	Keela::LabeledSpinButton::add(m_label);
}
