# Create the library first
add_library(gstkeelavideotestsrc SHARED
        gstkeelavideotestsrc.c
        gstkeelacustomplugin.c
)

# Link libraries and include directories
target_link_libraries(gstkeelavideotestsrc PkgConfig::GSTREAMER PkgConfig::GST_BASE PkgConfig::GST_VIDEO)

# Ensure filename matches GStreamer autoloader pattern: libgst*.so
set_target_properties(gstkeelavideotestsrc PROPERTIES
        OUTPUT_NAME "keelavideotestsrc"
        PREFIX "lib"
)

# Define required macros
target_compile_options(gstkeelavideotestsrc PRIVATE ${GST_CFLAGS})
target_compile_definitions(gstkeelavideotestsrc PRIVATE
        PACKAGE="keela-videotestsrc"
        VERSION="1.0"
        PACKAGE_VERSION="1.0"
        GST_PACKAGE_NAME="Keela videotestsrc"
        GST_PACKAGE_ORIGIN="https://github.com/Evan-Hartley/Keela"
)

# Set plugin installation directory
set(PLUGIN_INSTALL_DIR ${CMAKE_BINARY_DIR}/plugins)
install(TARGETS gstkeelavideotestsrc
        LIBRARY DESTINATION ${PLUGIN_INSTALL_DIR}
)

# Copy plugin to installation directory after build
add_custom_command(TARGET gstkeelavideotestsrc POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PLUGIN_INSTALL_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:gstkeelavideotestsrc> ${PLUGIN_INSTALL_DIR}/
)
