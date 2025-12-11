cmake_minimum_required(VERSION 3.20)

project(SatelliteAntop)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(LIBRARY_NAME SatelliteAntopLib)
set(EXECUTABLE_NAME SatelliteAntop)
set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

include(FetchContent)

option(ENABLE_SANITIZERS "Enable Address / Undefined / Thread Sanitizers" OFF)
if(ENABLE_SANITIZERS)
    message(STATUS "Sanitizers enabled")
    include(LTO)
    include(Sanitizer)
    add_sanitizer_flags()
    include(Warnings)
endif()

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
install(DIRECTORY ${h3_SOURCE_DIR}/src/h3lib/include/ DESTINATION include/h3)

add_library(${LIBRARY_NAME} src/util.cpp src/address.cpp src/cell.cpp src/antop.cpp src/resolution.cpp src/routingTable.cpp)
target_include_directories(${LIBRARY_NAME} PUBLIC ${PROJECT_INCLUDE_DIR} ${h3_SOURCE_DIR}/src ${h3_SOURCE_DIR}/src/h3lib/include)
target_link_libraries(${LIBRARY_NAME} PUBLIC h3)

add_executable(${EXECUTABLE_NAME} src/main.cpp)
target_link_libraries(${EXECUTABLE_NAME} PUBLIC ${LIBRARY_NAME})

if(ENABLE_SANITIZERS)
    target_lto(${EXECUTABLE_NAME})
    target_lto(${LIBRARY_NAME})
    target_set_warnings(${EXECUTABLE_NAME})
    target_set_warnings(${LIBRARY_NAME})
endif()

# apply coverage flags to the library target (so consumers inherit)
target_compile_options(${LIBRARY_NAME} PUBLIC -coverage)
target_link_options(${LIBRARY_NAME} PUBLIC -coverage)

# and to the main executable as well
target_compile_options(${EXECUTABLE_NAME} PRIVATE -coverage)
target_link_options(${EXECUTABLE_NAME} PRIVATE -coverage)

install(TARGETS ${LIBRARY_NAME} DESTINATION lib)
install(DIRECTORY ${PROJECT_INCLUDE_DIR}/ DESTINATION include)



