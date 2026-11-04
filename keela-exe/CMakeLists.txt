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

target_link_libraries(Keela PRIVATE keela-widgets keela-pipeline PkgConfig::GSTREAMER PkgConfig::GTKMM PkgConfig::SPDLOG)