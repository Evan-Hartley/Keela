#include "keela-pipeline/IControlResolution.h"
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
