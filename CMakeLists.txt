CMAKE_MINIMUM_REQUIRED(VERSION 3.28)
project(Keela)
find_package(PkgConfig)

# GoogleTest requires at least C++17
# We need C++20 for jthreads
# need c++23 for std::byteswap
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wall)

option(MAKE_REFERENCE "Build reference gstreamer gtk integration" false)
option(ENABLE_KEELA_VIDEOTESTSRC "Enable custom Keela videotestsrc plugin for development/testing" false)
option(USE_HSLUV "Use HSLuv color mapping instead of basic HSL" false)

pkg_search_module(GTKMM REQUIRED IMPORTED_TARGET gtkmm-3.0)
pkg_search_module(GSTREAMER REQUIRED IMPORTED_TARGET gstreamer-1.0)
pkg_search_module(GST_VIDEO REQUIRED IMPORTED_TARGET gstreamer-video-1.0)
pkg_search_module(SPDLOG REQUIRED IMPORTED_TARGET spdlog)
pkg_search_module(GIO REQUIRED IMPORTED_TARGET gio-2.0)
pkg_search_module(ARAVIS REQUIRED IMPORTED_TARGET aravis-0.8)

# compile the keela-videotestsrc plugin if enabled
if (ENABLE_KEELA_VIDEOTESTSRC)
    pkg_search_module(GST_BASE REQUIRED IMPORTED_TARGET gstreamer-base-1.0)
    add_compile_definitions(KEELA_VIDEOTESTSRC_ENABLED)
    add_subdirectory(keela-videotestsrc)
endif ()

if (USE_HSLUV)
    add_compile_definitions(KEELA_USE_HSLUV)
endif ()
enable_testing()
add_subdirectory(vendor)
add_subdirectory(keela-widgets)
add_subdirectory(keela-pipeline)
add_subdirectory(keela-exe)
add_subdirectory(keela-test)
add_subdirectory(reference)

# Add a target to clean configuration when switching video test source modes
add_custom_target(clean-config
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
        COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeCache.txt
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Clean CMake configuration - use this when switching ENABLE_KEELA_VIDEOTESTSRC"
)
