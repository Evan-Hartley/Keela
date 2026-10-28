add_executable(
    DriverInstaller
    DriverInstaller.cpp
)

target_link_libraries(
    DriverInstaller
    PRIVATE
        libwdi::libwdi
)

install(
    TARGETS DriverInstaller
    RUNTIME DESTINATION .
)