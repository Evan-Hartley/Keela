//
// Created by brand on 12/20/2025.
//

#ifndef PIXFMTCOMBO_H
#define PIXFMTCOMBO_H
#include "AravisController.h"
#include "cameramanager.h"
#include "labeledcomboboxtext.h"
namespace Keela {
class PixFmtCombo final : public Keela::LabeledComboBoxText {
   public:
	PixFmtCombo(std::shared_ptr<CameraManager> cameramanager);
	~PixFmtCombo() override;

	/// use this function to trigger this control to query araviscontroller for updated pixel format list
	void init();

   private:
	std::shared_ptr<CameraManager> m_camera_manager;

	void on_combo_changed() const;
};
}  // namespace Keela
#endif  // PIXFMTCOMBO_H
