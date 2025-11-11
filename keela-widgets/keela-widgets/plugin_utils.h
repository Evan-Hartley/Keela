//
// Helper functions for using custom Keela plugins
//

#ifndef KEELA_PLUGIN_UTILS_H
#define KEELA_PLUGIN_UTILS_H

#include <aravis-0.8/arv.h>
#include <gstreamer-1.0/gst/gst.h>

#include <string>

namespace Keela {
/**
 * Check if the custom keelavideotestsrc plugin is available
 */
bool is_custom_videotestsrc_available();

/**
 * Check if aravissrc plugin is available (plugin only, not camera)
 */
bool is_aravissrc_plugin_available();

/**
 * Test and report camera detection status
 */
bool aravis_camera_connected();

/**
 * Get the appropriate video source element name.
 * Preference order: aravissrc (with camera) > keelavideotestsrc > videotestsrc
 */
std::string get_video_source_name();
}  // namespace Keela

#endif  // KEELA_PLUGIN_UTILS_H
