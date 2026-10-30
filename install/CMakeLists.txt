set(ENV{PKG_CONFIG_PATH} ${DEPS_INSTALL_DIR}/lib/pkgconfig)

#include pcre2
add_library(PCRE2 SHARED IMPORTED)

set_target_properties(
    PCRE2
    PROPERTIES
        IMPORTED_IMPLIB
                "${DEPS_INSTALL_DIR}/lib/libpcre2-8.lib"
            
        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/bin/libpcre2-8.dll"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/pcre2"
)

#include gio
add_library(GIO SHARED IMPORTED)

set_target_properties(
    GIO
    PROPERTIES
        IMPORTED_IMPLIB
            "${DEPS_INSTALL_DIR}/lib/libgio-2.0.dll.a"

        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/bin/libgio-2.0.dll"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/glib-2.0"
)

#include gobject
add_library(GObject SHARED IMPORTED)

set_target_properties(
    GObject
    PROPERTIES
        IMPORTED_IMPLIB
            "${DEPS_INSTALL_DIR}/lib/gobject-2.0.dll.a"

        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/bin/gobject-2.0.dll"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/glib-2.0"
)

#include glib
add_library(GLib SHARED IMPORTED)

set_target_properties(
    GLib
    PROPERTIES
        IMPORTED_IMPLIB
            "${DEPS_INSTALL_DIR}/lib/glib-2.0.dll.a"

        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/bin/glib-2.0.dll"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/glib-2.0"
)

#include gtk
add_library(GTK3 SHARED IMPORTED)

set_target_properties(
    GTK3
    PROPERTIES
        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/lib/libgtk-3.dll.a"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/gtk-3.0"
)

#include ffmpeg
add_library(FFmpegAvutil SHARED IMPORTED)

set_target_properties(
    FFmpegAvutil
    PROPERTIES
        IMPORTED_IMPLIB
            "${DEPS_INSTALL_DIR}/lib/avutil.lib"

        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/bin/avutil.dll"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/ffmpeg"
)

add_library(FFmpegAvfilter SHARED IMPORTED)

set_target_properties(
    FFmpegAvfilter
    PROPERTIES
        IMPORTED_IMPLIB
            "${DEPS_INSTALL_DIR}/lib/avfilter.lib"

        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/bin/avfilter.dll"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/ffmpeg"
)

add_library(FFmpegAvcodec SHARED IMPORTED)

set_target_properties(
    FFmpegAvcodec
    PROPERTIES
        IMPORTED_IMPLIB
            "${DEPS_INSTALL_DIR}/lib/avcodec.lib"

        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/bin/avcodec.dll"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/ffmpeg"
)

add_library(FFmpegAvformat SHARED IMPORTED)

set_target_properties(
    FFmpegAvformat
    PROPERTIES
        IMPORTED_IMPLIB
            "${DEPS_INSTALL_DIR}/lib/avformat.lib"

        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/bin/avformat.dll"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/ffmpeg"
)

add_library(FFmpegSwscale SHARED IMPORTED)

set_target_properties(
    FFmpegSwscale
    PROPERTIES
        IMPORTED_IMPLIB
            "${DEPS_INSTALL_DIR}/lib/swscale.lib"

        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/bin/swscale.dll"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/ffmpeg"
)

add_library(FFmpegSwresample SHARED IMPORTED)

set_target_properties(
    FFmpegSwresample
    PROPERTIES
        IMPORTED_IMPLIB
            "${DEPS_INSTALL_DIR}/lib/swresample .lib"

        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/bin/swresample .dll"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/ffmpeg"
)

#include gstreamer
add_library(GStreamer SHARED IMPORTED)

set_target_properties(
    GStreamer
    PROPERTIES
        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/lib/libgstreamer-1.0.dll.a"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/gstreamer-1.0"
)

#include gstreamer-base
add_library(GStreamerBase SHARED IMPORTED)

set_target_properties(
    GStreamerBase
    PROPERTIES
        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/lib/libgstbase-1.0.dll.a"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/gstreamer-1.0"
)

#include gstreamer-video
add_library(GStreamerVideo SHARED IMPORTED)

set_target_properties(
    GStreamerVideo
    PROPERTIES
        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/lib/libgstvideo-1.0.dll.a"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/gstreamer-1.0"
)

#include gstreamer-audio
add_library(GStreamerAudio SHARED IMPORTED)

set_target_properties(
    GStreamerAudio
    PROPERTIES
        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/lib/libgstaudio-1.0.dll.a"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/gstreamer-1.0"
)

#include gstreamer-app
add_library(GStreamerApp SHARED IMPORTED)

set_target_properties(
    GStreamerApp
    PROPERTIES
        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/lib/libgstapp-1.0.dll.a"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/gstreamer-1.0"
)

#include aravis
add_library(Aravis SHARED IMPORTED)

set_target_properties(
    Aravis
    PROPERTIES
        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/lib/libaravis-0.8.dll.a"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/libaravis-0.8"
)

#include aravis-gst
add_library(AravisGst SHARED IMPORTED)

set_target_properties(
    AravisGst
    PROPERTIES
        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/lib/libaravisgsst-0.8.dll.a"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/libaravis-0.8"

        INTERFACE_LINK_LIBRARIES
            Aravis::Aravis
)

#include gtkmm
add_library(GTKMM SHARED IMPORTED)

set_target_properties(
    GTKMM
    PROPERTIES
        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/lib/libgtkmm-3.0.dll.a"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/gtkmm-3.0"
)

#include tbb
add_library(TBB SHARED IMPORTED)

set_target_properties(
    TBB
    PROPERTIES
        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/lib/libtbb.dll.a"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/tbb"
)