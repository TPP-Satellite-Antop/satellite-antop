cmake_policy(SET CMP0135 NEW)

include(FetchContent)
include(GoogleTest)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(SatelliteAntopTest example.cpp)

target_link_libraries(SatelliteAntopTest PRIVATE
        GTest::gtest_main
)

gtest_discover_tests(SatelliteAntopTest)
