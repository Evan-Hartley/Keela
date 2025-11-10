//
// Created by brand on 11/9/2025.
//

#ifndef ICONTROLCAMERARESOLUTION_H
#define ICONTROLCAMERARESOLUTION_H
#include <cstdint>
#include <limits>

#include "keela-pipeline/transformbin.h"

namespace Keela {
    class IControlCameraResolution {
    public:
        virtual ~IControlCameraResolution() = default;

        virtual void SetResolution(uint32_t width, uint32_t height) = 0;

        virtual void SetBinning(uint32_t binning) = 0;

        virtual uint32_t GetBinning() = 0;

        virtual uint32_t GetBinningMin() = 0;

        virtual uint32_t GetBinningMax() = 0;

        /// reset any binning settings such that they produce no effect (usually in preparation for switching to a different binning strategy)
        void ResetCameraResolution() {
            SetBinning(1);
        };
    };
}
#endif //ICONTROLCAMERARESOLUTION_H
