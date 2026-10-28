#ifndef __GST_KEELA_VIDEO_TEST_SRC_H__
#define __GST_KEELA_VIDEO_TEST_SRC_H__

#include <gst/base/gstpushsrc.h>
#include <gst/gst.h>
#include <gst/video/video.h>

G_BEGIN_DECLS

#define GST_TYPE_KEELA_VIDEO_TEST_SRC (gst_keela_video_test_src_get_type())
#define GST_KEELA_VIDEO_TEST_SRC(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_KEELA_VIDEO_TEST_SRC, GstKeelaVideoTestSrc))
#define GST_KEELA_VIDEO_TEST_SRC_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_KEELA_VIDEO_TEST_SRC, GstKeelaVideoTestSrcClass))
#define GST_IS_KEELA_VIDEO_TEST_SRC(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_KEELA_VIDEO_TEST_SRC))
#define GST_IS_KEELA_VIDEO_TEST_SRC_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_KEELA_VIDEO_TEST_SRC))

typedef struct _GstKeelaVideoTestSrc GstKeelaVideoTestSrc;
typedef struct _GstKeelaVideoTestSrcClass GstKeelaVideoTestSrcClass;

struct _GstKeelaVideoTestSrc {
	GstPushSrc element;

	gint64 accum_frames;

	/* video state */
	GstVideoInfo info;
	gint64 accum_rtime;
};

struct _GstKeelaVideoTestSrcClass {
	GstPushSrcClass parent_class;
};

GType gst_keela_video_test_src_get_type(void);

G_END_DECLS

#endif /* __GST_KEELA_VIDEO_TEST_SRC_H__ */
