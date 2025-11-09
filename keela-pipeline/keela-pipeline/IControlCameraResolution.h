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

        /// reset any binning settings to their defaults (usually in preparation for switching to a different binning strategy)
        virtual void ResetCameraResolution() = 0;
    };

    /*
    class SoftwareControlCameraResolution : public IControlCameraResolution {
    public:
        explicit SoftwareControlCameraResolution(Keela::TransformBin &transformbin);

        void SetResolution(uint32_t width, uint32_t height) override;

        void SetBinning(uint32_t binning) override;

        uint32_t GetBinning() override { return m_binning_factor; };

        uint32_t GetBinningMin() override { return std::numeric_limits<uint32_t>::min(); };

        uint32_t GetBinningMax() override { return std::numeric_limits<uint32_t>::max(); };

        void ResetCameraResolution() override;

    private:
        uint32_t m_binning_factor = 1;
        uint32_t m_resolution_width = 720;
        uint32_t m_resolution_height = 540;
        Keela::TransformBin &m_transformbin;
    };*/
}
#endif //ICONTROLCAMERARESOLUTION_H
