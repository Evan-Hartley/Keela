#include libwdi
add_library(Libwdi SHARED IMPORTED)

set_target_properties(
    Libwdi
    PROPERTIES
        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/bin/libwdi.dll"

        IMPORTED_IMPLIB
            "${DEPS_INSTALL_DIR}/lib/libwdi.dll.a"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include"
)

#include gtkmm
add_library(GTKMM SHARED IMPORTED)

set_target_properties(
    GTKMM
    PROPERTIES
        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/lib/libgtkmm-3.0.dll.a"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include"
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

#include glib
add_library(GLib SHARED IMPORTED)

set_target_properties(
    GLib
    PROPERTIES
        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/lib/glib-2.0.dll.a"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/glib-2.0"
)

#include gio
add_library(GIO SHARED IMPORTED)

set_target_properties(
    GIO
    PROPERTIES
        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/lib/libgio-2.0.dll.a"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/glib-2.0"
)

set(DOXYGEN_EXECUTABLE
    ${DEPS_INSTALL_DIR}/bin/doxygen
)
