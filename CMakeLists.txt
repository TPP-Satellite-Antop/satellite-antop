cmake_minimum_required(VERSION 3.20)

project(SatelliteAntop)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(LIBRARY_NAME SatelliteAntopLib)
set(EXECUTABLE_NAME SatelliteAntop)
set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

include(FetchContent)

option(ENABLE_TESTING "Enable a Unit Testing Build" ON)
if(ENABLE_TESTING)
    FetchContent_Declare(googletest GIT_REPOSITORY https://github.com/google/googletest.git GIT_TAG v1.16.0 GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(googletest)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif ()

FetchContent_Declare(h3 GIT_REPOSITORY https://github.com/uber/h3.git GIT_TAG v4.2.1 GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(h3)

add_library(${LIBRARY_NAME} src/address.cpp)
target_include_directories(${LIBRARY_NAME} PUBLIC ${PROJECT_INCLUDE_DIR} ${h3_SOURCE_DIR}/src)
target_link_libraries(${LIBRARY_NAME} PUBLIC h3)

add_executable(${EXECUTABLE_NAME} src/main.cpp)
target_link_libraries(${EXECUTABLE_NAME} PUBLIC ${LIBRARY_NAME})
