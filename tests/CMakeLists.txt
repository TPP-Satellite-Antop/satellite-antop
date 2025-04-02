include(GoogleTest)

set(TEST_MAIN "SatelliteAntopTest")
set(TEST_SOURCES "example.cpp")
set(TEST_INCLUDES "./")
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE) # config for Windows

add_executable(${TEST_MAIN} ${TEST_SOURCES})
target_include_directories(${TEST_MAIN} PUBLIC ${TEST_INCLUDES})
target_link_libraries(${TEST_MAIN} PUBLIC ${LIBRARY_NAME} GTest::gtest_main)

gtest_discover_tests(${TEST_MAIN})
