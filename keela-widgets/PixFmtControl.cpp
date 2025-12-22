//
// Created by brand on 12/20/2025.
//

#include "keela-widgets/PixFmtControl.h"

#include <spdlog/spdlog.h>

#include "keela-pipeline/consts.h"

Keela::PixFmtControl::PixFmtControl(std::shared_ptr<CameraManager> cameramanager)
    : Keela::FrameBox("Pixel Format Control", Gtk::ORIENTATION_VERTICAL),
      m_camera_manager(cameramanager),
      m_format_combo("Pixel Format"),
      m_depth_spin("Bit Depth") {
	m_depth_spin.m_spin.set_adjustment(Gtk::Adjustment::create(8, 8, 16));

	on_params_changed();
	m_depth_spin.m_spin.signal_changed().connect(sigc::mem_fun(*this, &PixFmtControl::on_params_changed));
	FrameBox::add(m_format_combo);
	FrameBox::add(m_depth_spin);
}

Keela::PixFmtControl::~PixFmtControl() {
}
void Keela::PixFmtControl::restart() {
	m_camera_manager->set_pix_fmt(m_format_combo.m_combo.get_active_id());
	// TODO: set bit depth
}

void Keela::PixFmtControl::on_params_changed() {
	if(m_depth_spin.m_spin.get_value() == 8) {
		// desired bit-depth is 8
		m_format_combo.m_combo.remove_all();
		m_format_combo.m_combo.append(GRAY8, "GRAY8");
		m_format_combo.m_combo.set_active_id(GRAY8);
		m_format_combo.set_sensitive(false);
	} else {
		// desired bit-depth does not fit in 8 bits
		m_format_combo.set_sensitive(true);
		m_format_combo.m_combo.remove_all();
		m_format_combo.m_combo.append(GRAY16_LE, "GRAY16 (Little Endian)");
		m_format_combo.m_combo.append(GRAY16_BE, "GRAY16 (Big Endian)");

		// if previous active ID was GRAY8, then set it to GRAY16_LE by default
		if(m_format_combo.m_combo.get_active_id() != GRAY8) {
			m_format_combo.m_combo.set_active_id(GRAY16_LE);
		}
	}
}
void Keela::PixFmtControl::start_recording() {
	this->set_sensitive(false);
	IRecordable::start_recording();
}
void Keela::PixFmtControl::stop_recording() {
	this->set_sensitive(true);
	IRecordable::stop_recording();
}