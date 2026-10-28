#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>

#include "gstkeelavideotestsrc.h"

static gboolean
plugin_init(GstPlugin* plugin) {
    gboolean ret = FALSE;

    ret |= gst_element_register(plugin, "keelavideotestsrc",
                                GST_RANK_NONE, GST_TYPE_KEELA_VIDEO_TEST_SRC);

    return ret;
}

GST_PLUGIN_DEFINE(GST_VERSION_MAJOR,
                  GST_VERSION_MINOR,
                  keelavideotestsrc,
                  "Keela Video Test Source plugin",
                  plugin_init, "1.0", "LGPL", "Keela", "https://github.com/Evan-Hartley/Keela")
