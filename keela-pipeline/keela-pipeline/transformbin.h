//
// Created by brand on 5/26/2025.
//

#ifndef TRANSFORMBIN_H
#define TRANSFORMBIN_H
#include "caps.h"
#include "queuebin.h"
#include "simpleelement.h"
#include "bin.h"
#include "IControlCameraResolution.h"

namespace Keela {
    class TransformBin final : public QueueBin, public IControlCameraResolution {
    public:
        TransformBin();

        /**
         *
         * @param name Name of bin
         * @param subsample whether to subsample the video stream
         */
        explicit TransformBin(const std::string &name);

        ~TransformBin() override;

        void flip_horizontal(bool apply_flip) const;

        void flip_vertical(bool apply_flip) const;

        void rotate_identity() const;

        void rotate_90() const;

        void rotate_180() const;

        void rotate_270() const;


        /**
         * scale video by 1/2 in each direction using the provided base resolution
         * @param width base width
         * @param height base height
         */
        [[deprecated("Use software binning instead")]]
        void scale(int width, int height);

    private:
        const std::string IDENTITY = "identity";
        const std::string FLIP_HORIZONTAL = "horiz";
        const std::string FLIP_VERTICAL = "vert";
        const std::string ROTATE_90 = "90r";
        const std::string ROTATE_180 = "180";
        const std::string ROTATE_270 = "90l";

        /// name of the gobject property to look up enum variant values
        const std::string FLIP_PROP = "video-direction";
        /*
         * video-direction     : Video direction: rotation and flipping
                    flags: readable, writable, controllable, changeable in NULL, READY, PAUSED or PLAYING state
                    Enum "GstVideoOrientationMethod" Default: 0, "identity"
                       (0): identity         - GST_VIDEO_ORIENTATION_IDENTITY
                       (1): 90r              - GST_VIDEO_ORIENTATION_90R
                       (2): 180              - GST_VIDEO_ORIENTATION_180
                       (3): 90l              - GST_VIDEO_ORIENTATION_90L
                       (4): horiz            - GST_VIDEO_ORIENTATION_HORIZ
                       (5): vert             - GST_VIDEO_ORIENTATION_VERT
                       (6): ul-lr            - GST_VIDEO_ORIENTATION_UL_LR
                       (7): ur-ll            - GST_VIDEO_ORIENTATION_UR_LL
                       (8): auto             - GST_VIDEO_ORIENTATION_AUTO
                       (9): custom           - GST_VIDEO_ORIENTATION_CUSTOM
         */
        void rotate(const std::string &rotation) const;

        void init() override;

        void link() override;

    public:
        void SetResolution(uint32_t width, uint32_t height) override;

        void SetBinning(uint32_t binning) override;

        uint32_t GetBinning() override { return binning_factor };

        uint32_t GetBinningMin() override { return std::numeric_limits<uint32_t>::min(); };

        uint32_t GetBinningMax() override { return std::numeric_limits<uint32_t>::max(); };

        void ResetCameraResolution() override;

    private:
        Keela::SimpleElement video_scale = SimpleElement("videoscale");
        Keela::SimpleElement scaled_caps_filter = SimpleElement("capsfilter", "scaled");
        Keela::SimpleElement native_caps_filter = SimpleElement("capsfilter", "native");
        Keela::Caps caps;
        Keela::SimpleElement rotation = SimpleElement("videoflip");
        Keela::SimpleElement flip_h = SimpleElement("videoflip");
        Keela::SimpleElement flip_v = SimpleElement("videoflip");

        uint32_t binning_factor = 1;
    };
}
#endif //TRANSFORMBIN_H
