CMAKE_MINIMUM_REQUIRED(VERSION 3.28)
project(Keela-Master)

include(ExternalProject)
include(ProcessorCount)
ProcessorCount(NPROC)

set(DEPS_VENDOR_DIR ${PROJECT_SOURCE_DIR}/vendor)
set(DEPS_INSTALL_DIR '${PROJECT_SOURCE_DIR}/install')
set(DEPS_PKCONFIG_DIR '${PROJECT_SOURCE_DIR}/install/lib/pkgconfig')

#external libraries - build step

ExternalProject_Add(
    pcre2

    SOURCE_DIR
        '${DEPS_VENDOR_DIR}/pcre2'

    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX='${DEPS_INSTALL_DIR}'
        -DPCRE2_BUILD_PCRE2_8=ON
        -DPCRE2_BUILD_PCRE2_16=OFF
        -DPCRE2_BUILD_PCRE2_32=OFF

    INSTALL_DIR
        '${DEPS_INSTALL_DIR}'
)

ExternalProject_Add(
    glib

    SOURCE_DIR
        '${DEPS_VENDOR_DIR}/glib'

    DEPENDS
        pcre2

    CONFIGURE_COMMAND
        env
        PKG_CONFIG_PATH=${DEPS_PKCONFIG_DIR}
        meson setup
        <BINARY_DIR>
        <SOURCE_DIR>
        --prefix=${DEPS_INSTALL_DIR}

    BUILD_COMMAND
        ninja -C <BINARY_DIR>

    INSTALL_COMMAND
        ninja -C <BINARY_DIR> install
)

ExternalProject_Add(
    gtk

    SOURCE_DIR
        '${DEPS_VENDOR_DIR}/gtk'

    DEPENDS
        glib

    CONFIGURE_COMMAND
        env
        PKG_CONFIG_PATH=${DEPS_PKCONFIG_DIR}
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
    ffmpeg

    SOURCE_DIR
        '${DEPS_VENDOR_DIR}/ffmpeg'

    INSTALL_DIR
        ${DEPS_INSTALL_DIR}

    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${DEPS_INSTALL_DIR}

    CONFIGURE_COMMAND
        bash -c "cd <SOURCE_DIR>"
        bash -c "./configure --enable-shared --disable-static"
        bash -c "cd .."
        bash -c "cd .."

    BUILD_COMMAND
        bash -c "cd <SOURCE_DIR>"
        bash -c "make -j${NPROC}"
        bash -c "cd .."
        bash -c "cd .."

    INSTALL_COMMAND
        bash -c "cd <SOURCE_DIR>"
        bash -c "make install"
        bash -c "cd .."
        bash -c "cd .."
)

ExternalProject_Add(
    gstreamer

    SOURCE_DIR
        '${DEPS_VENDOR_DIR}/gstreamer'
    
    DEPENDS
        glib
        ffmpeg

    CONFIGURE_COMMAND
        env
        PKG_CONFIG_PATH=${DEPS_PKCONFIG_DIR}
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
        '${DEPS_VENDOR_DIR}/gst-plugins-base'

    DEPENDS
        glib
        ffmpeg
        gstreamer

    CONFIGURE_COMMAND
        env
        PKG_CONFIG_PATH=${DEPS_PKCONFIG_DIR}
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
        '${DEPS_VENDOR_DIR}/gst-plugins-good'

    DEPENDS
        glib
        ffmpeg
        gstreamer
        gst_plugins_base

    CONFIGURE_COMMAND
        env
        PKG_CONFIG_PATH=${DEPS_PKCONFIG_DIR}
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
        '${DEPS_VENDOR_DIR}/gst-plugins-bad'

    DEPENDS
        glib
        ffmpeg
        gstreamer
        gst_plugins_base
        gst_plugins_good

    CONFIGURE_COMMAND
        env
        PKG_CONFIG_PATH=${DEPS_PKCONFIG_DIR}
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
        '${DEPS_VENDOR_DIR}/gst-plugins-ugly'

    INSTALL_DIR
        {DEPS_INSTALL_DIR}

    DEPENDS
        glib
        ffmpeg
        gstreamer
        gst_plugins_base
        gst_plugins_good
        gst_plugins_bad

    CONFIGURE_COMMAND
        env
        PKG_CONFIG_PATH=${DEPS_PKCONFIG_DIR}
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
        '${DEPS_VENDOR_DIR}/gst-libav'

    DEPENDS
        glib
        ffmpeg
        gstreamer
        gst_plugins_base
        gst_plugins_good
        gst_plugins_bad
        gst_plugins_ugly

    CONFIGURE_COMMAND
        env
        PKG_CONFIG_PATH=${DEPS_PKCONFIG_DIR}
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
        '${DEPS_VENDOR_DIR}/aravis'

    DEPENDS
        glib
        ffmpeg
        gstreamer

    CONFIGURE_COMMAND
        env
        PKG_CONFIG_PATH=${DEPS_PKCONFIG_DIR}
        meson setup
        <BINARY_DIR>
        <SOURCE_DIR>
        --prefix=${DEPS_INSTALL_DIR}
        -Dgst-plugin=enable

    BUILD_COMMAND
        meson compile -C <BINARY_DIR>

    INSTALL_COMMAND
        meson install -C <BINARY_DIR>
)

ExternalProject_Add(
    gtkmm

    SOURCE_DIR
        '${DEPS_VENDOR_DIR}/gtkmm-3-24'

    DEPENDS
        glib
        ffmpeg
        gstreamer
        aravis

    CONFIGURE_COMMAND
        env
        PKG_CONFIG_PATH=${DEPS_PKCONFIG_DIR}
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
    tbb

    SOURCE_DIR
        '${DEPS_VENDOR_DIR}/tbb'

    INSTALL_DIR
        ${DEPS_INSTALL_DIR}

    BUILD_COMMAND
        bash -c "cd '<SOURCE_DIR>'"
        bash -c "mkdir build && cd build"
        bash -c "cmake -DCMAKE_INSTALL_PREFIX='${DEPS_INSTALL_DIR} -DTBB_TEST=OFF -DTBB_STRICT=OFF .."
        bash -c "cmake --build"
        bash -c "cd .."
        bash -c "cd .."

    INSTALL_COMMAND
        bash -c "cd '<SOURCE_DIR>'"
        bash -c "cmake --install"
        bash -c "cd .."
        bash -c "cd .."
)

# cmake supported libraries - fetch step
add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/glad)
add_subdirectory(install)

# create a third-party bundle
add_custom_target(third_party)
add_dependencies(
    third_party
    pcre2
    glib
    gtk
    gstreamer
    gst_plugins_base
    gst_plugins_good
    gst_plugins_bad
    gst_plugins_ugly
    gst_libav
    aravis
    gtkmm
    tbb
)

ExternalProject_Add(
    keela_ready

    SOURCE_DIR
        '${PROJECT_SOURCE_DIR}/keela-exe'

    DEPENDS
        third_party

    CMAKE_ARGS
        -DDEPS_INSTALL_DIR=${DEPS_INSTALL_DIR}
)


add_dependencies(
    keela_ready
    third_party
)

# Install dependecies in the installation-setup folder
install(
    DIRECTORY
        '${DEPS_INSTALL_DIR}/bin/'
    DESTINATION
        '${PROJECT_SOURCE_DIR}/KeelaSetup/bin'
)

install(
    DIRECTORY
        '${DEPS_INSTALL_DIR}/lib/'
    DESTINATION
        '${PROJECT_SOURCE_DIR}/KeelaSetup/lib'
)

install(
    DIRECTORY
        ${DEPS_INSTALL_DIR}/lib/pkgconfig
    DESTINATION
        ${PROJECT_SOURCE_DIR}/KeelaSetup/lib/pkgconfig
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