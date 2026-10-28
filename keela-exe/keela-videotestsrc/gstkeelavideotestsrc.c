#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/base/gstpushsrc.h>
#include <gst/gst.h>
#include <gst/video/video.h>
#include <math.h>
#include <string.h>

#include "gstkeelavideotestsrc.h"

enum {
    PROP_0
};

static GstStaticPadTemplate gst_keela_video_test_src_template =
    GST_STATIC_PAD_TEMPLATE("src",
                            GST_PAD_SRC,
                            GST_PAD_ALWAYS,
                            GST_STATIC_CAPS(GST_VIDEO_CAPS_MAKE("GRAY8")));

#define gst_keela_video_test_src_parent_class parent_class
G_DEFINE_TYPE(GstKeelaVideoTestSrc, gst_keela_video_test_src, GST_TYPE_PUSH_SRC);

static void gst_keela_video_test_src_finalize(GObject *object);

static gboolean gst_keela_video_test_src_setcaps(GstBaseSrc *bsrc,
                                                 GstCaps *caps);
static GstFlowReturn gst_keela_video_test_src_fill(GstPushSrc *psrc,
                                                   GstBuffer *buffer);
static gboolean gst_keela_video_test_src_start(GstBaseSrc *basesrc);
static gboolean gst_keela_video_test_src_stop(GstBaseSrc *basesrc);
static GstCaps *gst_keela_video_test_src_get_caps(GstBaseSrc *bsrc,
                                                  GstCaps *filter);
static GstCaps *gst_keela_video_test_src_fixate(GstBaseSrc *bsrc,
                                                GstCaps *caps);

/* Helper function declarations */
static void fill_ball_pattern(GstKeelaVideoTestSrc *src, guint8 *dest, GstClockTime timestamp);

static void
gst_keela_video_test_src_class_init(GstKeelaVideoTestSrcClass *klass) {
    GObjectClass *gobject_class;
    GstElementClass *gstelement_class;
    GstBaseSrcClass *gstbasesrc_class;
    GstPushSrcClass *gstpushsrc_class;

    gobject_class = (GObjectClass *)klass;
    gstelement_class = (GstElementClass *)klass;
    gstbasesrc_class = (GstBaseSrcClass *)klass;
    gstpushsrc_class = (GstPushSrcClass *)klass;

    gobject_class->finalize = gst_keela_video_test_src_finalize;

    gst_element_class_set_static_metadata(gstelement_class,
                                          "Keela Ball Test Source", "Source/Video",
                                          "Creates a moving ball pattern for motion detection testing",
                                          "Keela Team");

    gst_element_class_add_static_pad_template(gstelement_class,
                                              &gst_keela_video_test_src_template);

    gstbasesrc_class->get_caps = gst_keela_video_test_src_get_caps;
    gstbasesrc_class->fixate = gst_keela_video_test_src_fixate;
    gstbasesrc_class->set_caps = gst_keela_video_test_src_setcaps;
    gstbasesrc_class->start = gst_keela_video_test_src_start;
    gstbasesrc_class->stop = gst_keela_video_test_src_stop;

    gstpushsrc_class->fill = gst_keela_video_test_src_fill;
}

static void
gst_keela_video_test_src_init(GstKeelaVideoTestSrc *src) {
    gst_base_src_set_format(GST_BASE_SRC(src), GST_FORMAT_TIME);
    gst_base_src_set_live(GST_BASE_SRC(src), FALSE);
}

static void
gst_keela_video_test_src_finalize(GObject *object) {
    G_OBJECT_CLASS(parent_class)->finalize(object);
}

static GstCaps *
gst_keela_video_test_src_get_caps(GstBaseSrc *bsrc, GstCaps *filter) {
    GstPad *pad = GST_BASE_SRC_PAD(bsrc);
    return gst_pad_get_pad_template_caps(pad);
}

static GstCaps *
gst_keela_video_test_src_fixate(GstBaseSrc *bsrc, GstCaps *caps) {
    GstStructure *structure;

    caps = gst_caps_make_writable(caps);

    structure = gst_caps_get_structure(caps, 0);

    gst_structure_fixate_field_nearest_int(structure, "width", 320);
    gst_structure_fixate_field_nearest_int(structure, "height", 240);
    gst_structure_fixate_field_nearest_fraction(structure, "framerate", 500, 1);

    caps = GST_BASE_SRC_CLASS(parent_class)->fixate(bsrc, caps);

    return caps;
}

static gboolean
gst_keela_video_test_src_setcaps(GstBaseSrc *bsrc, GstCaps *caps) {
    GstKeelaVideoTestSrc *src = GST_KEELA_VIDEO_TEST_SRC(bsrc);
    gboolean ret = FALSE;

    if (gst_video_info_from_caps(&src->info, caps)) {
        src->accum_rtime = 0;
        ret = TRUE;
    }

    return ret;
}

static gboolean
gst_keela_video_test_src_start(GstBaseSrc *basesrc) {
    GstKeelaVideoTestSrc *src = GST_KEELA_VIDEO_TEST_SRC(basesrc);

    src->accum_frames = 0;
    src->accum_rtime = 0;

    return TRUE;
}

static gboolean
gst_keela_video_test_src_stop(GstBaseSrc *basesrc) {
    return TRUE;
}

/* Helper function to fill a ball pattern */
static void
fill_ball_pattern(GstKeelaVideoTestSrc *src, guint8 *dest, GstClockTime timestamp) {
    gint width = src->info.width;
    gint height = src->info.height;
    gint ball_radius = 20; /* Fixed ball size of 40 pixels diameter */
    gint center_x, center_y;
    guint8 bg_color, ball_color;

    /* Fancy bouncing movement - ball moves in a wavy pattern */
    gdouble time_sec = (gdouble)timestamp / GST_SECOND;
    center_x = (gint)(width / 2 + (width / 4) * sin(time_sec * 2.0));
    center_y = (gint)(height / 2 + (height / 4) * sin(time_sec * 1.5));

    /* Ensure ball stays within bounds */
    center_x = CLAMP(center_x, ball_radius, width - ball_radius);
    center_y = CLAMP(center_y, ball_radius, height - ball_radius);

    /* Alternate colors every frame to simulate experimental conditions */
    if (src->accum_frames % 2 == 0) {
        bg_color = 0;     /* Black background */
        ball_color = 255; /* White ball */
    } else {
        bg_color = 255; /* White background */
        ball_color = 0; /* Black ball */
    }

    /* Fill background */
    memset(dest, bg_color, width * height);

    /* Draw ball */
    gint stride = GST_VIDEO_INFO_PLANE_STRIDE(&src->info, 0);
    for (gint y = 0; y < height; y++) {
        for (gint x = 0; x < width; x++) {
            gint dx = x - center_x;
            gint dy = y - center_y;
            gint distance_sq = dx * dx + dy * dy;

            if (distance_sq <= ball_radius * ball_radius) {
                /* Inside the ball - set to ball color */
                guint8 *pixel = dest + y * stride;
                pixel[x] = ball_color;
            }
        }
    }
}

static GstFlowReturn
gst_keela_video_test_src_fill(GstPushSrc *psrc, GstBuffer *buffer) {
    GstKeelaVideoTestSrc *src;
    GstClockTime next_time;
    GstMapInfo map;

    src = GST_KEELA_VIDEO_TEST_SRC(psrc);

    if (G_UNLIKELY(src->info.fps_n <= 0 || src->info.width <= 0 ||
                   src->info.height <= 0))
        return GST_FLOW_NOT_NEGOTIATED;

    /* generate the next time to output this buffer */
    next_time = src->accum_rtime;

    GST_BUFFER_TIMESTAMP(buffer) = next_time;
    GST_BUFFER_DURATION(buffer) = src->info.fps_d * GST_SECOND / src->info.fps_n;
    GST_BUFFER_OFFSET(buffer) = src->accum_frames;
    GST_BUFFER_OFFSET_END(buffer) = src->accum_frames + 1;

    if (!gst_buffer_map(buffer, &map, GST_MAP_WRITE))
        return GST_FLOW_ERROR;

    /* Fill the buffer with ball pattern */
    fill_ball_pattern(src, map.data, next_time);

    gst_buffer_unmap(buffer, &map);

    src->accum_frames++;
    src->accum_rtime = next_time + GST_BUFFER_DURATION(buffer);

    return GST_FLOW_OK;
}
