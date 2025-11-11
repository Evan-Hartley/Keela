#pragma once

namespace Keela {
/**
 * Initialize GStreamer plugins and verify custom plugins are available.
 * Should be called after gst_init() but before creating any elements.
 */
void initialize_plugins();
}  // namespace Keela
