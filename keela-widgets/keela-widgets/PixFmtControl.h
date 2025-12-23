//
// Created by brand on 12/20/2025.
//

#ifndef PIXFMTCOMBO_H
#define PIXFMTCOMBO_H
#include "AravisController.h"
#include "IRecordable.h"
#include "IRestartable.h"
#include "cameramanager.h"
#include "framebox.h"
#include "labeledcomboboxtext.h"
#include "labeledspinbutton.h"
namespace Keela {

class PixFmtControl final : public Keela::FrameBox, public IRestartable, public IRecordable {
   public:
	explicit PixFmtControl(std::shared_ptr<CameraManager> cameramanager);
	~PixFmtControl() override;

	sigc::signal<void,std::string> signal_pix_fmt_changed;

   private:
	std::shared_ptr<CameraManager> m_camera_manager;

	void on_params_changed();

   public:
	void start_recording() override;
	void stop_recording() override;
	void restart() override;

   private:
	Keela::LabeledComboBoxText m_format_combo;
	Keela::LabeledSpinButton m_depth_spin;
};
}  // namespace Keela
#endif  // PIXFMTCOMBO_H
