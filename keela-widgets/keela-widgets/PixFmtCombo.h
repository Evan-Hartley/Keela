//
// Created by brand on 12/20/2025.
//

#ifndef PIXFMTCOMBO_H
#define PIXFMTCOMBO_H
#include "AravisController.h"
#include "labeledcomboboxtext.h"
namespace Keela {
class PixFmtCombo final : public Keela::LabeledComboBoxText {
   public:
	PixFmtCombo();
	~PixFmtCombo() override;

	/// use this function to trigger this control to query araviscontroller for updated pixel format list
	void init();

   private:
	AravisController **m_araviscontroller;

	void on_combo_changed();
};
}  // namespace Keela
#endif  // PIXFMTCOMBO_H
