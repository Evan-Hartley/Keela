#include "keela-pipeline/IControlResolution.h"
void Keela::IControlResolution::SetResolution(Resolution1D res) {
  // memorize the resolution setting for this dimension
  switch (res.dimension) {
  case Width:
    res_width = std::make_unique<Resolution1D>(res);
    break;
  case Height:
    res_height = std::make_unique<Resolution1D>(res);
    break;
  }

  if (res_width) {
    doSetBinning(*res_width);
    doSetResolution(*res_width);
  }

  if (res_height) {
    doSetBinning(*res_height);
    doSetResolution(*res_height);
  }
}
//
// Created by brand on 11/10/2025.
//
std::vector<Keela::IControlResolution::Resolution1D>
Keela::IControlResolution::GetResolutions(Dimension dimension) {
  auto res_max = GetResolutionMax(dimension);
  auto res_min = GetResolutionMin(dimension);
  auto binning_min = GetBinningMin(dimension);
  auto binning_max = GetBinningMax(dimension);

  std::vector<Resolution1D> res_vec;

  for (auto i = binning_min; i <= binning_max; i++) {
    auto scaled = std::floor(res_max / static_cast<double>(res_min));
    if (scaled >= res_min) {
      Resolution1D res = {.value = static_cast<uint32_t>(scaled),
                          .binning = i,
                          .dimension = dimension};
      res_vec.push_back(res);
    } else {
      // no point in using higher binning values
      break;
    }
  }
  return res_vec;
}
