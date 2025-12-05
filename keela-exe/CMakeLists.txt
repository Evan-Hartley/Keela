add_executable(keela main.cpp mainwindow.cpp mainwindow.h
        cameracontrolwindow.cpp
        cameracontrolwindow.h
        tracewindow.cpp
        tracewindow.h)


target_link_libraries(keela PRIVATE keela-widgets keela-pipeline PkgConfig::GSTREAMER PkgConfig::GTKMM PkgConfig::SPDLOG)

install(TARGETS keela)