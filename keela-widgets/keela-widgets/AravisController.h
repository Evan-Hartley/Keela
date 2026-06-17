#ifndef ARAVISCONTROLLER_H
#define ARAVISCONTROLLER_H

#include <aravis-0.8/arv.h>
#include <gst/gstelement.h>

#include <string>
#include <utility>  // pair
#include <vector>

namespace Keela {
/**
 * Controls Aravis camera operations and settings
 *
 * This class abstracts all Aravis-specific camera controls,
 * keeping camera management separate from pipeline management.
 */
class AravisController {
   public:
	explicit AravisController(GstElement *camera);

	~AravisController() = default;

	/** |-----------------------------| */
	/** v Query hardware capabilities v */

	/**
	 * Get the range of frame rate values supported by the camera hardware
	 *
	 * Returns {nan, nan} if no aravis camera is available
	 */
	std::pair<double, double> get_framerate_range() const;

	double get_framerate() const;

	/**
	 * Get the range of gain values supported by the camera hardware
	 *
	 * Returns {nan, nan} if no aravis camera is available
	 */
	std::pair<double, double> get_gain_range() const;

	double get_gain() const;

	/**
	 * Get the range of exposure time values supported by the camera hardware
	 *
	 * Returns {nan, nan} if no aravis camera is available
	 */
	std::pair<double, double> get_exposure_time_range() const;

	double get_exposure_time() const;

	bool supports_hardware_binning() const;

	/**
	 * Get the binning modes (e.g. "Sum", "Average") supported by the camera hardware
	 *
	 * Note: this method only checks for the modes of the "BinningHorizontalMode" feature.
	 */
	std::vector<std::string> get_supported_binning_modes();

	/**
	 * Get the current binning modes (horizontal, vertical)
	 *
	 * Returns {"", ""} if no aravis camera is available
	 */
	std::pair<std::string, std::string> get_binning_modes() const;

	/**
	 * Get the range of binning factor values supported by the camera hardware
	 *
	 * Returns a tuple of {min_x_binning, max_x_binning, min_y_binning, max_y_binning}
	 *
	 * Returns {nan, nan, nan, nan} if no aravis camera is available
	 */
	std::tuple<int, int, int, int> get_binning_bounds() const;

	std::pair<int, int> get_binning_factors() const;

	/**
	 * Get the increment steps for binning factor values supported by the camera hardware
	 *
	 * Returns {nan, nan} if no aravis camera is available
	 */
	std::pair<int, int> get_binning_increments() const;

	/**
	 * Get the current Width and Height of the camera sensor area
	 * This is the effective resolution after any binning/ROI cropping has been applied
	 *
	 * Returns {nan, nan} if no aravis camera is available
	 */
	std::pair<int, int> get_current_resolution() const;

	std::vector<std::string> get_available_pixel_formats() const;
	void set_pixel_format(std::string format);

	/** ^ Query hardware capabilities ^ */
	/** |-----------------------------| */

	/** |---------------------------| */
	/** v Control hardware settings v */

	void set_gain(double gain);

	void set_exposure_time(double exposure);

	/**
	 * Note: pipeline needs to be reset after use
	 *       aravissrc doesn't dynamically handle 'feature' changes
	 */
	void set_binning_mode(const std::string &mode);

	/**
	 * Note: pipeline needs to be reset after use
	 *       aravissrc doesn't dynamically handle 'feature' changes
	 */
	void set_binning_factors(int binning_factor);
	void set_binning_factors(int binning_factor_x, int binning_factor_y);

	/** ^ Control hardware settings ^ */
	/** |---------------------------| */

   private:
	GstElement *aravis_source;
	ArvCamera *aravis_camera;

	// placeholders to "remember" our feature settings for generating feature strings
	std::string feature_pix_fmt;
	std::string feature_binning_mode;
	void set_features();
};
}  // namespace Keela

#endif  // ARAVISCONTROLLER_H
