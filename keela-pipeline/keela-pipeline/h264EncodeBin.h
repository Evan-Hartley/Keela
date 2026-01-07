//
// Created by brand on 1/7/26.
//

#ifndef KEELA_H264ENCODEBIN_H
#define KEELA_H264ENCODEBIN_H
#include "bin.h"
#include "elementbase.h"
#include "simpleelement.h"
#include "VideoEncoder.h"

namespace Keela {
    class H264EncodeBin: public Keela::VideoEncoder, public Keela::Bin{
    public:
        H264EncodeBin();
        ~H264EncodeBin() override = default;

    private:
        void init() override;

        void link() override;

        SimpleElement caps_filter = SimpleElement("capsfilter");
        SimpleElement video_convert = SimpleElement("videoconvert");
        SimpleElement enc = SimpleElement("x264enc");

        static GstPadProbeReturn caps_probe(GstPad *pad, GstPadProbeInfo *probeinfo,void* user_data);
    };
}
#endif //KEELA_H264ENCODEBIN_H