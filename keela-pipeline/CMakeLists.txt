add_library(keela-pipeline presentationbin.cpp keela-pipeline/presentationbin.h
        recordbin.cpp
        keela-pipeline/recordbin.h
        transformbin.cpp
        keela-pipeline/transformbin.h
        bin.cpp
        keela-pipeline/bin.h
        keela-pipeline/gst-helpers.h
        gst-helpers.cpp
        queuebin.cpp
        keela-pipeline/queuebin.h
        simpleelement.cpp
        keela-pipeline/simpleelement.h
        elementbase.cpp
        keela-pipeline/elementbase.h
        gtkglsink.cpp
        keela-pipeline/gtkglsink.h
        gtksink.cpp
        keela-pipeline/gtksink.h
        keela-pipeline/widgetelement.h
        keela-pipeline/utils.h
        utils.cpp
        caps.cpp
        keela-pipeline/caps.h
        snapshotbin.cpp
        keela-pipeline/snapshotbin.h
        TraceBin.cpp
        keela-pipeline/TraceBin.h
        EjectableElement.cpp
        keela-pipeline/EjectableElement.h
        keela-pipeline/consts.h
        CameraStreamBin.cpp
        keela-pipeline/CameraStreamBin.h
        keela-pipeline/IControlCameraResolution.h
)
target_link_libraries(keela-pipeline PUBLIC PkgConfig::GSTREAMER PkgConfig::SPDLOG)

target_include_directories(keela-pipeline PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})