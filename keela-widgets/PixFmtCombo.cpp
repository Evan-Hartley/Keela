//
// Created by brand on 12/20/2025.
//

#include "keela-widgets/PixFmtCombo.h"

#include <spdlog/spdlog.h>

Keela::PixFmtCombo::PixFmtCombo(std::shared_ptr<CameraManager> cameramanager)
    : LabeledComboBoxText("Pixel Format"), m_camera_manager(cameramanager) {
	m_combo.signal_changed().connect(sigc::mem_fun(*this, &PixFmtCombo::on_combo_changed));
	m_combo.set_sensitive(false);
}

Keela::PixFmtCombo::~PixFmtCombo() {
}
void Keela::PixFmtCombo::init() {
	if(!m_camera_manager) {
		spdlog::warn("PixFmtCombo::{}: Camera manager is uninitialized", __func__);
		return;
	}
	auto fmts = m_camera_manager->get_available_pixel_formats();
	m_combo.remove_all();
	if(!fmts.empty()) {
		for(auto fmt : fmts) {
			m_combo.append(fmt);
		}
		m_combo.set_active_text(fmts.at(0));
		m_combo.set_sensitive(true);
	}
}
void Keela::PixFmtCombo::on_combo_changed() const {
	if(!m_camera_manager) {
		spdlog::warn("Camera manager is uninitialized");
		return;
	}
	m_camera_manager->set_pix_fmt(m_combo.get_active_text());
}