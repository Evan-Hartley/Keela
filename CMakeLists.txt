CMAKE_MINIMUM_REQUIRED(VERSION 3.28)
project(Keela)
find_package(TBB REQUIRED)
find_package(Doxygen COMPONENTS dot)
# GoogleTest requires at least C++17
# We need C++20 for jthreads
# need c++23 for std::byteswap
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wall)

option(MAKE_REFERENCE "Build reference gstreamer gtk integration" false)
option(ENABLE_KEELA_VIDEOTESTSRC "Enable custom Keela videotestsrc plugin for development/testing" false)
option(USE_HSLUV "Use HSLuv color mapping instead of basic HSL" false)
option(USE_FFV1 "Use FFV1 codec to encode video. If disabled, keela will use h.264" true)

find_package(gtkmm-3.0 REQUIRED CONFIG)
find_package(gstreamer-1.0 REQUIRED CONFIG)
find_package(gstreamer-video-1.0 REQUIRED CONFIG)
find_package(spdlog REQUIRED CONFIG)
add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE) # generate SPDLOG_* logging macros for this logging level and above
find_package(gio-2.0 REQUIRED CONFIG)
find_package(aravis-0.8 REQUIRED CONFIG)

# compile the keela-videotestsrc plugin if enabled
if (ENABLE_KEELA_VIDEOTESTSRC)
    find_package(gstreamer-base-1.0 REQUIRED CONFIG)
    add_compile_definitions(KEELA_VIDEOTESTSRC_ENABLED)
    add_subdirectory(keela-videotestsrc)
endif ()

if (USE_HSLUV)
    add_compile_definitions(KEELA_USE_HSLUV)
endif ()

if (USE_FFV1)
    message("Enabling FFV1 codec")
    add_compile_definitions(KEELA_USE_FFV1)
endif ()

enable_testing()
add_subdirectory(vendor)
add_subdirectory(keela-widgets)
add_subdirectory(keela-pipeline)
add_subdirectory(keela-exe)
add_subdirectory(keela-test)
add_subdirectory(reference)
add_subdirectory(vendor/glad)

# Add a target to clean configuration when switching video test source modes
add_custom_target(clean-config
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
        COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeCache.txt
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Clean CMake configuration - use this when switching ENABLE_KEELA_VIDEOTESTSRC"
)

if (Doxygen_FOUND)
    doxygen_add_docs(docs readme.md keela-exe keela-widgets keela-pipeline keela-videotestsrc)
endif ()

# setup a installer file
set(CPACK_PACKAGE_NAME "Keela")
set(CPACK_PACKAGE_VENDOR "Georgia Institute of Techology - CHAOS Lab")
set(CPACK_PACKAGE_VERSION "1.0.0")

set(CPACK_PACKAGE_INSTALL_DIRECTORY "Keela")

set(CPACK_GENORATOR "NSIS")

set(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut $DESKTOP\\Keela.lnk $INSTDIR\\Keela.exe")

set(CPACK_NSIS_DELETE_ICONS_EXTRA "Delete $DESKTOP\\Keela.lnk")

include(CPack)