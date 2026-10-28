add_executable(
    DriverInstaller
    DriverInstaller.cpp
)

add_dependencies(DriverInstaller libwdi)

target_link_libraries(
    DriverInstaller
    PRIVATE
        Libwdi
)

install(
    TARGETS DriverInstaller
    RUNTIME DESTINATION .
)