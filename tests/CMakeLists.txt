cmake_minimum_required(VERSION 3.20)

include(GoogleTest)
include(Sanitizer)
add_sanitizer_flags()

set(CMAKE_CXX_STANDARD_REQUIRED True)
set(TEST_MAIN SatelliteAntopTest)
set(TEST_SOURCES Address.cpp distance.cpp h3util.cpp Cell.cpp Hypercube.cpp RoutingTable.cpp)

add_executable(${TEST_MAIN} ${TEST_SOURCES})
target_link_libraries(${TEST_MAIN} PUBLIC ${LIBRARY_NAME} GTest::gtest_main)

target_compile_options(${TEST_MAIN} PRIVATE -coverage)
target_link_options(${TEST_MAIN} PRIVATE -coverage)

gtest_discover_tests(${TEST_MAIN})
