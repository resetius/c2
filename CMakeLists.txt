project(c2)

cmake_minimum_required(VERSION 2.4)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(FindJPEG)
include(Flex)
include(Bison)
include(fftw3)

add_definitions(-fopenmp)

set(LIBRARY_OUTPUT_PATH "./lib")
set(EXECUTABLE_OUTPUT_PATH "./bin")

add_subdirectory(filters)
add_subdirectory(fractal)
add_subdirectory(vizualizer)

