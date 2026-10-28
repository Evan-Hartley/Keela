

if (MAKE_REFERENCE)
    add_dependencies(keela)
    add_executable(gst-tutorial-5 gst-tutorial-5.c)
    target_link_libraries(gst-tutorial-5 PRIVATE GStreamer::GStreamer gtk::gtk)

    if (TARGET GStreamer::Audio)
        add_executable(gst-tutorial-8 gst-tutorial-8.c)
        target_link_libraries(gst-tutorial-8 PRIVATE GStreamer::GStreamer GStreamer::Audio)
    endif ()
    add_executable(setterminate setterminate.cpp)
    add_executable(pad-probe-dynamic-pipeline pad-probe-dynamicpipeline.c)
    target_link_libraries(pad-probe-dynamic-pipeline GStreamer::GStreamer)
    #add_executable(gst_jpg_on_demand gst_jpg_on_demand.cpp)
    #target_link_libraries(gst_jpg_on_demand PRIVATE GStreamer::GStreamer)
else ()
    message("Skipping gstreamer gui toolkit reference implementation")
endif ()