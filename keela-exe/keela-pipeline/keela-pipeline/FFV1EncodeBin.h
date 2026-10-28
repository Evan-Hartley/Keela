//
// Created by brand on 1/7/26.
//

#ifndef KEELA_FFV1ENCODEBIN_H
#define KEELA_FFV1ENCODEBIN_H
#include "bin.h"
#include "simpleelement.h"
#include "VideoEncoder.h"

namespace Keela {
    class FFV1EncodeBin: public Keela::Bin, public Keela::VideoEncoder {
        public:
        FFV1EncodeBin();
         ~FFV1EncodeBin() override = default;

    private:
        void init() override;

        void link() override;

        SimpleElement video_convert = SimpleElement("videoconvert");
        SimpleElement caps_filter = SimpleElement("capsfilter");
        SimpleElement enc = SimpleElement("avenc_ffv1");
    };
}


#endif //KEELA_FFV1ENCODEBIN_H