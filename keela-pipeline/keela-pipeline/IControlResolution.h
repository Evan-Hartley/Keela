//
// Created by brand on 11/9/2025.
//

#ifndef ICONTROLCAMERARESOLUTION_H
#define ICONTROLCAMERARESOLUTION_H
#include "keela-pipeline/transformbin.h"
#include <cstdint>
#include <limits>
#include <vector>

namespace Keela {
/// An object that can manage and control the binning and resolution settings of
/// a camera
class IControlResolution {
public:
  enum Dimension {
    Width,
    Height,
  };
  /// A one dimensional resolution setting associated with a specific binning
  /// factor and direction
  struct Resolution1D {
    uint32_t value;
    uint32_t binning;
    Dimension dimension;
  };

  virtual ~IControlResolution() = default;

  void SetResolution(Resolution1D resolution_1d);

  // TODO: perhaps cache this?
  virtual uint32_t GetBinning(Dimension dimension) = 0;

private:
  std::unique_ptr<Resolution1D> res_width = nullptr;
  std::unique_ptr<Resolution1D> res_height = nullptr;

  virtual uint32_t GetBinningMin(Dimension dimension) = 0;

  virtual uint32_t GetBinningMax(Dimension dimension) = 0;

  // TODO: maybe make this protected?
  virtual void doSetBinning(Resolution1D resolution) = 0;

  /// the minimum physical resolution our camera supports in a particular
  /// dimension
  virtual uint32_t GetResolutionMin(Dimension dimension) = 0;

  /// the maximum physical resolution our camera supports in a particular
  /// dimension
  virtual uint32_t GetResolutionMax(Dimension dimension) = 0;

  // TODO: maybe make this protected?
  virtual void doSetResolution(Resolution1D resolution) = 0;

public:
  /// returns a list of valid resolution/binning settings
  std::vector<Resolution1D> GetResolutions(Dimension dimension);
};
} // namespace Keela
#endif // ICONTROLCAMERARESOLUTION_H
