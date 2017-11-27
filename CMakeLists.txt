cmake_minimum_required (VERSION 3.5)

project (modern-mutual-information)

set (CMAKE_CXX_STANDARD 11)

# Enable compiler warnings.
if ( MSVC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
else ( MSVC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")
endif ( MSVC )

# Use OpenMP for parallelizing calculations.
find_package(OpenMP)
if ( OPENMP_FOUND )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
else ()
    message(WARNING "OpenMP not supported! Calculations can not be parallelized!")
endif ()

include_directories("${PROJECT_SOURCE_DIR}/lib" "${PROJECT_SOURCE_DIR}/src")

file(GLOB SOURCES src/*.cpp)
file(GLOB TEST_SOURCES test/*.cpp)

# Copy test data to target directory.
configure_file("test/SimpleCSV_data1.csv" "test/SimpleCSV_data1.csv" COPYONLY)
configure_file("test/SimpleCSV_random1.csv" "test/SimpleCSV_random1.csv" COPYONLY)
configure_file("test/SimpleCSV_random2.csv" "test/SimpleCSV_random2.csv" COPYONLY)

add_executable(run_tests ${TEST_SOURCES} ${SOURCES})
add_executable(shiftmi main.cpp ${SOURCES})

# Automatically run tests after compilation.
add_custom_command(TARGET shiftmi POST_BUILD COMMAND run_tests)