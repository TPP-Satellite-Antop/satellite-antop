cmake_minimum_required(VERSION 3.20)

project(SatelliteAntop)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

enable_testing()

add_executable(SatelliteAntop src/main.cpp)
add_subdirectory(tests)
