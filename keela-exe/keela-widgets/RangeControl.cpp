//
// Created by brand on 12/23/25.
//

#include "keela-widgets/RangeControl.h"

#include "keela-pipeline/consts.h"

Keela::RangeControl::RangeControl(): FrameBox("Range", Gtk::ORIENTATION_VERTICAL) {
    FrameBox::add(range_check);
    range_spin_min.m_spin.set_digits(2);
    range_spin_max.m_spin.set_digits(2);
    FrameBox::add(range_spin_min);
    FrameBox::add(range_spin_max);
}

void Keela::RangeControl::on_pix_fmt_changed(std::string fmt) {
    uint32_t max = std::numeric_limits<uint16_t>::max();
    if(fmt == GRAY8) {
        max = std::numeric_limits<uint8_t>::max();
    }
    auto adj_max = Gtk::Adjustment::create(max, 0, max, 0.1);
    range_spin_max.m_spin.set_adjustment(adj_max);
    auto adj_min = Gtk::Adjustment::create(0.0, 0, max, 0.1);
    range_spin_min.m_spin.set_adjustment(adj_min);
    heatmap_scale = max;
}

void Keela::RangeControl::range_check_clicked() {
    bool checked = range_check.get_active();
    range_spin_min.set_sensitive(!checked);
    range_spin_max.set_sensitive(!checked);
}

bool Keela::RangeControl::is_heatmap_enabled() {
    return range_check.get_active();
}

float Keela::RangeControl::heatmap_min() {
    return range_spin_min.m_spin.get_value() / heatmap_scale;
}

float Keela::RangeControl::heatmap_max() {
    return range_spin_max.m_spin.get_value() / heatmap_scale;
}
