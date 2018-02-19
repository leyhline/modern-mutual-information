cmake_minimum_required (VERSION 3.5)

project (modern-mutual-information)

# Let's use modern C++
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

file(GLOB TEST_SOURCES test/*.cpp)

# Copy test data to target directory.
configure_file("test/SimpleCSV_data1.csv" "test/SimpleCSV_data1.csv" COPYONLY)
configure_file("test/SimpleCSV_random1.csv" "test/SimpleCSV_random1.csv" COPYONLY)
configure_file("test/SimpleCSV_random2.csv" "test/SimpleCSV_random2.csv" COPYONLY)
configure_file("test/SimpleBinaryFile_data1.bin" "test/SimpleBinaryFile_data1.bin" COPYONLY)
configure_file("test/SimpleBinaryFile_data2.bin" "test/SimpleBinaryFile_data2.bin" COPYONLY)

# One may use the provided Toolchain file to cross-compile for windows
# using the x86_64-w64-mingw32 compiler.
# For this call cmake with -DCMAKE_TOOLCHAIN_FILE=path/to/Toolchain-x86_64-w64-mingw32.cmake
IF(CMAKE_CROSSCOMPILING)
    SET(BUILD_SHARED_LIBRARIES OFF)
    SET(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
ENDIF(CMAKE_CROSSCOMPILING)

#add_executable(run_tests ${TEST_SOURCES})
add_executable(run_tests test/test_main.cpp test/Histogram1d_test.cpp)
add_executable(shiftmi main.cpp)

# Automatically run tests after compilation if possible.
IF(NOT CMAKE_CROSSCOMPILING)
    #add_custom_command(TARGET shiftmi POST_BUILD COMMAND run_tests)
	add_custom_command(TARGET run_tests POST_BUILD COMMAND run_tests)
ENDIF(NOT CMAKE_CROSSCOMPILING)
