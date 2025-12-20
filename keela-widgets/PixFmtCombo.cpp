//
// Created by brand on 12/20/2025.
//

#include "keela-widgets/PixFmtCombo.h"

#include <spdlog/spdlog.h>
Keela::PixFmtCombo::PixFmtCombo() : LabeledComboBoxText("Pixel Format") {
	m_combo.signal_changed().connect(*this, &PixFmtCombo::on_combo_changed);
}
Keela::PixFmtCombo::~PixFmtCombo() {
}
void Keela::PixFmtCombo::init() {
}
void Keela::PixFmtCombo::on_combo_changed() {
	// splitting this condition because I read once that the order of operations to evaluate a boolean expression is
	// undefined behavior (at least in C)
	//
	// these checks are not side-effect free (they can crash the program if they happen out of the intended order)
	if(!m_araviscontroller) {
		SPDLOG_WARN("Aravis Controller is uninitialized");
		return;
	}
	if(!*m_araviscontroller) {
		SPDLOG_WARN("Aravis Controller is uninitialized");
		return;
	}
	(*m_araviscontroller)->set_pixel_format(m_combo.get_active_text());
}