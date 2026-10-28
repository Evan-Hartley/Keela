#include libwdi
add_library(libwdi::libwdi SHARED IMPORTED)

set_target_properties(
    libwdi::libwdi
    PROPERTIES
        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/bin/libwdi.dll"

        IMPORTED_IMPLIB
            "${DEPS_INSTALL_DIR}/lib/libwdi.dll.a"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include"
)

#include gtkmm
add_library(gtkmm::gtkmm SHARED IMPORTED)

set_target_properties(
    gtkmm::gtkmm
    PROPERTIES
        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/lib/libgtkmm-3.0.dll.a"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include"
)

#include gtk
add_library(gtk::gtk UNKOWN IMPORTED)

set_target_properties(
    gtk::gtk
    PROPERTIES
        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/lib/libgtk-3.dll.a"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/gtk-3.0"
)

#include gstreamer
add_library(GStreamer::GStreamer SHARED IMPORTED)

set_target_properties(
    GStreamer::GStreamer
    PROPERTIES
        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/lib/libgstreamer-1.0.dll.a"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/gstreamer-1.0"
)

#include gstreamer-base
add_library(GStreamer::Base SHARED IMPORTED)

set_target_properties(
    GStreamer::Base
    PROPERTIES
        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/lib/libgstbase-1.0.dll.a"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/gstreamer-1.0"
)

#include gstreamer-video
add_library(GStreamer::Video SHARED IMPORTED)

set_target_properties(
    GStreamer::Video
    PROPERTIES
        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/lib/libgstvideo-1.0.dll.a"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/gstreamer-1.0"
)

#include gstreamer-audio
add_library(GStreamer::Audio SHARED IMPORTED)

set_target_properties(
    GStreamer::Audio
    PROPERTIES
        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/lib/libgstaudio-1.0.dll.a"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/gstreamer-1.0"
)

#include gstreamer-app
add_library(GStreamer::App SHARED IMPORTED)

set_target_properties(
    GStreamer::App
    PROPERTIES
        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/lib/libgstapp-1.0.dll.a"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/gstreamer-1.0"
)

#include aravis
add_library(Aravis::Aravis SHARED IMPORTED)

set_target_properties(
    Aravis::Aravis
    PROPERTIES
        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/lib/libaravis-0.8.dll.a"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/libaravis-0.8"
)

#include aravis-gst
add_library(Aravis::Gst UNKOWN IMPORT)

set_target_properties(
    Aravis::Gst
    PROPERTIES
        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/lib/libaravisgsst-0.8.dll.a"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/libaravis-0.8"

        INTERFACE_LINK_LIBRARIES
            Aravis::Aravis
)

#include glib
add_library(GLib::glib SHARED IMPORTED)

set_target_properties(
    glib::glib
    PROPERTIES
        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/lib/glib-2.0.dll.a"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/glib-2.0"
)

#include gio
add_library(GIO::gio SHARED IMPORTED)

set_target_properties(
    GIO::gio
    PROPERTIES
        IMPORTED_LOCATION
            "${DEPS_INSTALL_DIR}/lib/libgio-2.0.dll.a"
        
        INTERFACE_INCLUDE_DIRECTORIES
            "${DEPS_INSTALL_DIR}/include/glib-2.0"
)

set(DOXYGEN_EXECUTABLE
    ${DEPS_INSTALL_DIR}/bin/doxygen
)
