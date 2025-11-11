//
// Created by brand on 6/3/2025.
//
#include <gstreamer-1.0/gst/gst.h>
#include <gtest/gtest.h>

TEST(Reference, AddCapsFilterToBin) {
	auto bin = gst_bin_new(nullptr);
	auto filter = gst_element_factory_make("capsfilter", nullptr);
	auto sink = gst_element_factory_make("autovideosink", nullptr);
	gst_bin_add_many(GST_BIN(bin), filter, sink, nullptr);
	auto parent = gst_element_get_parent(filter);  // this refs parent which is an alias to bin
	g_object_unref(parent);
	ASSERT_EQ(GST_ELEMENT(parent), bin);
	auto ret = gst_element_link_many(filter, sink, nullptr);
	ASSERT_TRUE(ret);
	g_object_unref(bin);
}

TEST(Reference, ElementRefCountWhenAddedToBin) {
	// test the value of the element's ref counter after it has been added to the bin
	auto bin = gst_bin_new(nullptr);
	auto element = gst_element_factory_make("autovideosink", nullptr);
	gst_bin_add(GST_BIN(bin), element);
	EXPECT_TRUE(GST_OBJECT_REFCOUNT(element) == 1);
	g_object_unref(bin);
	EXPECT_FALSE(GST_IS_ELEMENT(element));
}
