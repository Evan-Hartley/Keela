CMAKE_MINIMUM_REQUIRED(VERSION 3.28)
project(Keela-Master)

include(ExternalProjects)

set(DEPS_VENDOR_DIR ${CMAKE_SOURCE_DIR}/vendor)
set(DEPS_INSTALL_DIR ${CMAKE_BINARY_DIR}/install)
set(DEPS_DRIVER_DIR ${CMAKE_SOURCE_DIR}/DriverInstaller)

#external libraries - build step
ExternalProject_Add(
    libwdi

    SOURCE_DIR
        ${CMAKE_VENDOR_DIR}/libwdi

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
        ${CMAKE_VENDOR_DIR}/gtkmm

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
        ${CMAKE_VENDOR_DIR}/gtk

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
        ${CMAKE_VENDOR_DIR}/gstreamer

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
    gst-plugins-base

    SOURCE_DIR
        ${CMAKE_VENDOR_DIR}/gst-plugins-base

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
    gst-plugins-good

    SOURCE_DIR
        ${CMAKE_VENDOR_DIR}/gst-plugins-good

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
    gst-plugins-bad

    SOURCE_DIR
        ${CMAKE_VENDOR_DIR}/gst-plugins-bad

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
    gst-plugins-ugly

    SOURCE_DIR
        ${CMAKE_VENDOR_DIR}/gst-plugins-ugly

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
    gst-libav

    SOURCE_DIR
        ${CMAKE_VENDOR_DIR}/gst-libav

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
        ${CMAKE_VENDOR_DIR}/aravis

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
    aravis-gst

    SOURCE_DIR
        ${CMAKE_VENDOR_DIR}/aravis/gst

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
        ${CMAKE_VENDOR_DIR}/glib

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
        ${CMAKE_VENDOR_DIR}/doxygen

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
    keela_exe

    SOURCE_DIR
        ${CMAKE_SOUCE_DIR}/keela

    CMAKE_ARGS
        -DCMAKE_PREFIX_PATH=${DEPS_INSTALL_DIR}
)

#cmake supported libraries - fetch step
add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/tbb)

#create a third-party bundle
add_custom_target(third_party)
add_dependencies(
    third_party
    gtkmm
    gtk
    gstreamer
    gst-plugins-base
    gst-plugins-good
    gst-plugins-bad
    gst-plugins-ugly
    gst-libav
    glib
    aravis
    aravis-gst
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
        ${CMAKE_SOURCE_DIR}/KeelaSetup/bin
)

install(
    DIRECTORY
        ${DEPS_INSTALL_DIR}/lib/
    DESTINATION
        ${CMAKE_SOURCE_DIR}/KeelaSetup/lib
)

install(
    DIRECTORY
        ${DEPS_INSTALL_DIR}/include/
    DESTINATION
        ${CMAKE_SOURCE_DIR}/KeelaSetup/include
)

install(
    DIRECTORY
        ${DEPS_INSTALL_DIR}/share/
    DESTINATION
        ${CMAKE_SOURCE_DIR}/KeelaSetup/share
)

install(
    DIRECTORY
        ${DEPS_INSTALL_DIR}/etc/
    DESTINATION
        ${CMAKE_SOURCE_DIR}/KeelaSetup/etc
)

#setup a installer file
set(CPACK_PACKAGE_NAME "Keela")
set(CPACK_PACKAGE_VENDOR "Georgia Institute of Techology - CHAOS Lab")
set(CPACK_PACKAGE_VERSION "1.0.0")

set(CPACK_PACKAGE_INSTALL_DIRECTORY "Keela")

set(CPACK_GENORATOR "NSIS")

set(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut '$DESKTOP\\Keela.lnk' '$INSTDIR\\keela.exe'")

set(CPACK_NSIS_DELETE_ICONS_EXTRA "Delete '$DESKTOP\\Keela.lnk'")

include(CPACK)