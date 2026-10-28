//
// Created by brand on 1/7/26.
//

#ifndef KEELA_VIDEOENCODER_H
#define KEELA_VIDEOENCODER_H
#include "elementbase.h"

namespace Keela {
    /// Marker class signifying an element that can encode a video stream.
    ///
    /// Subclasses are expected to perform any necessary format conversion internally
    class VideoEncoder: virtual public Keela::Element {};
}
#endif  // KEELA_VIDEOENCODER_H
