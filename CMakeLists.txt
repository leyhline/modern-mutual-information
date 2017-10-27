cmake_minimum_required (VERSION 3.5)
project (modern-mutual-information)

set (CMAKE_CXX_STANDARD 11)

include_directories("${PROJECT_BINARY_DIR}", "${PROJECT_SOURCE_DIR}/lib", "${PROJECT_SOURCE_DIR}/src")

file(GLOB SOURCES src/*.cpp)

add_executable(shiftmi ${SOURCES})