project(c2)

cmake_minimum_required(VERSION 2.8)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

find_package(JPEG)
include(fftw3)

add_definitions(-fopenmp)

set(LIBRARY_OUTPUT_PATH "./lib")
set(EXECUTABLE_OUTPUT_PATH "./bin")

add_subdirectory(filters)
add_subdirectory(fractal)

