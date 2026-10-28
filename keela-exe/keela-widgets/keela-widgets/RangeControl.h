//
// Created by brand on 12/23/25.
//

#ifndef KEELA_RANGECONTROL_H
#define KEELA_RANGECONTROL_H
#include "framebox.h"
#include "GLCameraRender.h"
#include "labeledspinbutton.h"

namespace Keela {
    class RangeControl: public Keela::FrameBox, public IControlGLCameraRenderHeatmap {
    public:
        RangeControl();
        ~RangeControl() override = default;

        void on_pix_fmt_changed(std::string fmt);
        private:
        LabeledSpinButton range_spin_min = LabeledSpinButton("Minimum");
        LabeledSpinButton range_spin_max = LabeledSpinButton("Maximum");
        Gtk::CheckButton range_check = Gtk::CheckButton("Range");
        double heatmap_scale;
        void range_check_clicked();

    public:
        bool is_heatmap_enabled() override;

        float heatmap_min() override;

        float heatmap_max() override;
    };
}
#endif //KEELA_RANGECONTROL_H