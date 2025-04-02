cmake_policy(SET CMP0135 NEW)

include(GoogleTest)

set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/refs/heads/main.zip)
FetchContent_MakeAvailable(googletest)

add_executable(SatelliteAntopTest example.cpp address.cpp)

target_link_libraries(SatelliteAntopTest PRIVATE SatelliteAntop GTest::gtest_main)

gtest_discover_tests(SatelliteAntopTest)
