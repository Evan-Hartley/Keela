

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/52eb8108c5bdec04579160ae17225d66034bd723.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(keela-test main.cpp pipeline.cpp
        keela-exe.cpp
        reference.cpp
        widgets.cpp
        gdk.cpp)

target_link_libraries(keela-test PRIVATE GTest::gtest keela-pipeline keela-widgets PkgConfig::GSTREAMER)

include(GoogleTest)
gtest_discover_tests(keela-test)