project(Keela)

# Set up executable
IF (WIN32)
  add_executable(Keela main.cpp mainwindow.cpp mainwindow.h
        cameracontrolwindow.cpp
        cameracontrolwindow.h
        tracewindow.cpp
        tracewindow.h
        keela-icon.o)
ELSE()
   add_executable(Keela main.cpp mainwindow.cpp mainwindow.h
        cameracontrolwindow.cpp
        cameracontrolwindow.h
        tracewindow.cpp
        tracewindow.h)
ENDIF()

# add dependencies incase of non-installation step
if(TARGET third_party)
    #build all dependencies before the executable
    add_dependencies(
        Keela
        third_party
    )
endif()

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

#all libraries - linking steps
target_link_libraries(
    Keela
    PRIVATE
        GTKMM
        GTK3
        GStreamer
        GStreamerBase
        GStreamerVideo
        GStreamerAudio
        GStreamerApp
        Aravis
        AravisGst
        GLib
        GIO
        spdlog::spdlog
        TBB::tbb
)

# generate SPDLOG_* logging macros for this logging level and above
add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

# compile the keela-videotestsrc plugin if enabled
if (ENABLE_KEELA_VIDEOTESTSRC)
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

# add necessary subdirectories
add_subdirectory(keela-widgets)
add_subdirectory(keela-pipeline)
add_subdirectory(keela-test)
add_subdirectory(reference)

# Add a target to clean configuration when switching video test source modes
add_custom_target(clean-config
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
        COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeCache.txt
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Clean CMake configuration - use this when switching ENABLE_KEELA_VIDEOTESTSRC"
)

if (Doxygen_FOUND)
    doxygen_add_docs(docs readme.md keela keela-widgets keela-pipeline keela-videotestsrc)
endif ()

install(
    TARGETS Keela
    RUNTIME DESTINATION ../KeelaSetup/
)