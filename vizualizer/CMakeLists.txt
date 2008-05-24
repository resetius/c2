project(calcs C CXX Fortran)
include(FindMPI)
include(FindOpenGL)
include(FindGLUT)
include(CheckLibraryExists)

include_directories(../asplib)
link_directories(../lib)

set(LIBRARY_OUTPUT_PATH "../lib")
set(EXECUTABLE_OUTPUT_PATH "../bin")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions(-D_DEBUG)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

if(CMAKE_COMPILER_IS_GNUG77)
	set(FFCLAGS g2c)
endif(CMAKE_COMPILER_IS_GNUG77)

if(CMAKE_Fortran_COMPILE STREQUAL "gfortran")
	set(FFCLAGS gfortran)
endif(CMAKE_Fortran_COMPILE STREQUAL "gfortran")

set(OGL ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})

add_executable(3Dviz3 viz_main.cpp viz_mainwindow.cpp
	viz_obj.cpp viz_console.cpp viz_event.cpp viz_surface.cpp
	viz_sphere.cpp viz_utils.c viz_triang.cpp)

target_link_libraries(3Dviz3 ${OGL} asp)

