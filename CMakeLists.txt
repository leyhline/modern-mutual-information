cmake_minimum_required (VERSION 3.5)

project (modern-mutual-information)

set (CMAKE_CXX_STANDARD 11)

include_directories("${PROJECT_SOURCE_DIR}/lib" "${PROJECT_SOURCE_DIR}/src")

file(GLOB SOURCES src/*.cpp)
file(GLOB TEST_SOURCES test/*.cpp)

add_executable(run_tests ${TEST_SOURCES} ${SOURCES})
add_executable(shiftmi main.cpp ${SOURCES})