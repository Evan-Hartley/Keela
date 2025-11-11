//
// Created by brand on 5/28/2025.
//

#ifndef GST_HELPERS_H
#define GST_HELPERS_H
#include <gst/gst.h>

#include <string>
namespace Keela {
gint gst_enum_variant_by_nick(GObject *obj, const std::string &propname, const std::string &nickname);
}
#endif  // GST_HELPERS_H
