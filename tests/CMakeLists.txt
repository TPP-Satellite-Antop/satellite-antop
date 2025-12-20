cmake_minimum_required(VERSION 3.20)

include(GoogleTest)
include(Sanitizer)
add_sanitizer_flags()

set(CMAKE_CXX_STANDARD_REQUIRED True)
set(TEST_MAIN SatelliteAntopTest)
set(TEST_SOURCES address.cpp util.cpp resolution.cpp cell.cpp antop.cpp routingTable.cpp)

add_executable(${TEST_MAIN} ${TEST_SOURCES})
target_link_libraries(${TEST_MAIN} PUBLIC ${LIBRARY_NAME} GTest::gtest_main)

target_compile_options(${TEST_MAIN} PRIVATE -coverage)
target_link_options(${TEST_MAIN} PRIVATE -coverage)

gtest_discover_tests(${TEST_MAIN})
