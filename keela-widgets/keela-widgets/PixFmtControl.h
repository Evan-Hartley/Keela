//
// Created by brand on 12/20/2025.
//

#ifndef PIXFMTCOMBO_H
#define PIXFMTCOMBO_H
#include "AravisController.h"
#include "cameramanager.h"
#include "framebox.h"
#include "labeledcomboboxtext.h"
#include "labeledspinbutton.h"
namespace Keela {
class PixFmtControl final : public Keela::FrameBox {
   public:
	explicit PixFmtControl(std::shared_ptr<CameraManager> cameramanager);
	~PixFmtControl() override;

	/// use this function to trigger this control to query araviscontroller for updated pixel format list
	// void init();

   private:
	std::shared_ptr<CameraManager> m_camera_manager;

	void on_params_changed();

	Keela::LabeledComboBoxText m_format_combo;
	Keela::LabeledSpinButton m_depth_spin;
};
}  // namespace Keela
#endif  // PIXFMTCOMBO_H
