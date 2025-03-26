cmake_minimum_required(VERSION 3.20)

project(SatelliteAntop)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)
FetchContent_Declare(h3 GIT_REPOSITORY https://github.com/uber/h3.git GIT_TAG v4.2.1)
FetchContent_MakeAvailable(h3)

enable_testing()

add_executable(SatelliteAntop src/main.cpp)
add_subdirectory(tests)

target_link_libraries(SatelliteAntop PRIVATE h3)
