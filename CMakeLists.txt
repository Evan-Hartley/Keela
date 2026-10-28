CMAKE_MINIMUM_REQUIRED(VERSION 3.28)
project(Keela-Master)

include(ExternalProject)

set(DEPS_VENDOR_DIR ${PROJECT_SOURCE_DIR}/vendor)
set(DEPS_INSTALL_DIR ${PROJECT_SOURCE_DIR}/install)
set(DEPS_DRIVER_DIR ${PROJECT_SOURCE_DIR}/DriverInstaller)

#external libraries - build step
ExternalProject_Add(
    libwdi

    SOURCE_DIR
        ${DEPS_VENDOR_DIR}/libwdi

    CONFIGURE_COMMAND
        ""

    BUILD_COMMAND
        msbuild
        <SOURCE_DIR>/libwdi.sln
        /p:Configuration=Release
        /p:Platform=x64

    INSTALL_COMMAND
        ${CMAKE_COMMAND} -E make_directory
        ${DEPS_INSTALL_DIR}/include

    COMMAND
        ${CMAKE_COMMAND} -E make_directory
        ${DEPS_INSTALL_DIR}/lib

    COMMAND
        ${CMAKE_COMMAND} -E copy_directory
        <SOURCE_DIR>/libwdi
        ${DEPS_INSTALL_DIR}/include/libwdi

    COMMAND
        ${CMAKE_COMMAND} -E copy
        <SOURCE_DIR>/msvc/x64/Release/libwdi.lib
        ${DEPS_INSTALL_DIR}/lib
)

ExternalProject_Add(
    gtkmm

    SOURCE_DIR
        ${DEPS_VENDOR_DIR}/gtkmm

    CONFIGURE_COMMAND
        meson setup
        <BINARY_DIR>
        <SOURCE_DIR>
        --prefix=${DEPS_INSTALL_DIR}

    BUILD_COMMAND
        meson compile -C <BINARY_DIR>

    INSTALL_COMMAND
        meson install -C <BINARY_DIR>
)

ExternalProject_Add(
    gtk

    SOURCE_DIR
        ${DEPS_VENDOR_DIR}/gtk

    CONFIGURE_COMMAND
        meson setup
        <BINARY_DIR>
        <SOURCE_DIR>
        --prefix=${DEPS_INSTALL_DIR}

    BUILD_COMMAND
        meson compile -C <BINARY_DIR>

    INSTALL_COMMAND
        meson install -C <BINARY_DIR>
)

ExternalProject_Add(
    gstreamer

    SOURCE_DIR
        ${DEPS_VENDOR_DIR}/gstreamer

    CONFIGURE_COMMAND
        meson setup
        <BINARY_DIR>
        <SOURCE_DIR>
        --prefix=${DEPS_INSTALL_DIR}

    BUILD_COMMAND
        meson compile -C <BINARY_DIR>

    INSTALL_COMMAND
        meson install -C <BINARY_DIR>
)

ExternalProject_Add(
    gst_plugins_base

    SOURCE_DIR
        ${DEPS_VENDOR_DIR}/gst-plugins-base

    CONFIGURE_COMMAND
        meson setup
        <BINARY_DIR>
        <SOURCE_DIR>
        --prefix=${DEPS_INSTALL_DIR}

    BUILD_COMMAND
        meson compile -C <BINARY_DIR>

    INSTALL_COMMAND
        meson install -C <BINARY_DIR>
)

ExternalProject_Add(
    gst_plugins_good

    SOURCE_DIR
        ${DEPS_VENDOR_DIR}/gst-plugins-good

    CONFIGURE_COMMAND
        meson setup
        <BINARY_DIR>
        <SOURCE_DIR>
        --prefix=${DEPS_INSTALL_DIR}

    BUILD_COMMAND
        meson compile -C <BINARY_DIR>

    INSTALL_COMMAND
        meson install -C <BINARY_DIR>
)

ExternalProject_Add(
    gst_plugins_bad

    SOURCE_DIR
        ${DEPS_VENDOR_DIR}/gst-plugins-bad

    CONFIGURE_COMMAND
        meson setup
        <BINARY_DIR>
        <SOURCE_DIR>
        --prefix=${DEPS_INSTALL_DIR}

    BUILD_COMMAND
        meson compile -C <BINARY_DIR>

    INSTALL_COMMAND
        meson install -C <BINARY_DIR>
)

ExternalProject_Add(
    gst_plugins_ugly

    SOURCE_DIR
        ${DEPS_VENDOR_DIR}/gst-plugins-ugly

    CONFIGURE_COMMAND
        meson setup
        <BINARY_DIR>
        <SOURCE_DIR>
        --prefix=${DEPS_INSTALL_DIR}

    BUILD_COMMAND
        meson compile -C <BINARY_DIR>

    INSTALL_COMMAND
        meson install -C <BINARY_DIR>
)

ExternalProject_Add(
    gst_libav

    SOURCE_DIR
        ${DEPS_VENDOR_DIR}/gst-libav

    CONFIGURE_COMMAND
        meson setup
        <BINARY_DIR>
        <SOURCE_DIR>
        --prefix=${DEPS_INSTALL_DIR}

    BUILD_COMMAND
        meson compile -C <BINARY_DIR>

    INSTALL_COMMAND
        meson install -C <BINARY_DIR>
)

ExternalProject_Add(
    aravis

    SOURCE_DIR
        ${DEPS_VENDOR_DIR}/aravis

    CONFIGURE_COMMAND
        meson setup
        <BINARY_DIR>
        <SOURCE_DIR>
        --prefix=${DEPS_INSTALL_DIR}

    BUILD_COMMAND
        meson compile -C <BINARY_DIR>

    INSTALL_COMMAND
        meson install -C <BINARY_DIR>
)

ExternalProject_Add(
    aravis_gst

    SOURCE_DIR
        ${DEPS_VENDOR_DIR}/aravis/gst

    CONFIGURE_COMMAND
        meson setup
        <BINARY_DIR>
        <SOURCE_DIR>
        --prefix=${DEPS_INSTALL_DIR}

    BUILD_COMMAND
        meson compile -C <BINARY_DIR>

    INSTALL_COMMAND
        meson install -C <BINARY_DIR>
)

ExternalProject_Add(
    glib

    SOURCE_DIR
        ${DEPS_VENDOR_DIR}/glib

    CONFIGURE_COMMAND
        meson setup
        <BINARY_DIR>
        <SOURCE_DIR>
        --prefix=${DEPS_INSTALL_DIR}

    BUILD_COMMAND
        meson compile -C <BINARY_DIR>

    INSTALL_COMMAND
        meson install -C <BINARY_DIR>
)

ExternalProject_Add(
    doxygen

    SOURCE_DIR
        ${DEPS_VENDOR_DIR}/doxygen

    CONFIGURE_COMMAND
        meson setup
        <BINARY_DIR>
        <SOURCE_DIR>
        --prefix=${DEPS_INSTALL_DIR}

    BUILD_COMMAND
        meson compile -C <BINARY_DIR>

    INSTALL_COMMAND
        meson install -C <BINARY_DIR>
)

# cmake supported libraries - fetch step
add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/tbb)
add_subdirectory(vendor/glad)

# create a third-party bundle
add_custom_target(third_party)
add_dependencies(
    third_party
    gtkmm
    gtk
    gstreamer
    gst_plugins_base
    gst_plugins_good
    gst_plugins_bad
    gst_plugins_ugly
    gst_libav
    glib
    aravis
    aravis_gst
    doxygen
)

#  Add the application subfolder
add_subdirectory(install)
add_subdirectory(DriverInstaller)
add_subdirectory(keela-exe)

# Install required camera driver
install(CODE [[
    message(STATUS "Running DriverInstaller)

    execute_process(
        COMMAND "${DEPS_DRIVER_DIR}/DriverInstaller.exe"
        RESULT_VARIABLE DRIVER_RESULT
    )

    if(NOT DRIVER_RESULT EQUAL 0)
        message(FATAL_ERROR
            "Driver installation failed with code ${DRIVER_RESULT}"
        )
    endif()
]])

# Install dependecies in the installation-setup folder
install(
    DIRECTORY
        ${DEPS_INSTALL_DIR}/bin/
    DESTINATION
        ${PROJECT_SOURCE_DIR}/KeelaSetup/bin
)

install(
    DIRECTORY
        ${DEPS_INSTALL_DIR}/lib/
    DESTINATION
        ${PROJECT_SOURCE_DIR}/KeelaSetup/lib
)

install(
    DIRECTORY
        ${DEPS_INSTALL_DIR}/include/
    DESTINATION
        ${PROJECT_SOURCE_DIR}/KeelaSetup/include
)

install(
    DIRECTORY
        ${DEPS_INSTALL_DIR}/share/
    DESTINATION
        ${PROJECT_SOURCE_DIR}/KeelaSetup/share
)

install(
    DIRECTORY
        ${DEPS_INSTALL_DIR}/etc/
    DESTINATION
        ${PROJECT_SOURCE_DIR}/KeelaSetup/etc
)

# setup a installer file
set(CPACK_PACKAGE_NAME "Keela")
set(CPACK_PACKAGE_VENDOR "Georgia Institute of Techology - CHAOS Lab")
set(CPACK_PACKAGE_VERSION "1.0.0")

set(CPACK_PACKAGE_INSTALL_DIRECTORY "Keela")

set(CPACK_GENORATOR "NSIS")

set(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut '$DESKTOP\\Keela.lnk' '$INSTDIR\\Keela.exe'")

set(CPACK_NSIS_DELETE_ICONS_EXTRA "Delete '$DESKTOP\\Keela.lnk'")

include(CPack)