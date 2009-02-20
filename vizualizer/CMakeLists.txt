project(calcs C CXX Fortran)
include(FindMPI)
include(FindOpenGL)
include(FindGLUT)
include(CheckLibraryExists)
include(UsePkgConfig)

pkgconfig(glib-2.0 GLIB2_INC_DIR GLIB2_LINK_DIR GLIB2_LINK_FLAGS
	                GLIB2_CFLAGS)

set(LIBRARY_OUTPUT_PATH "../lib")
set(EXECUTABLE_OUTPUT_PATH "../bin")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions(-D_DEBUG)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

add_definitions(${GLIB2_CFLAGS})
link_directories(${glib2LibDir})

set(OGL ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})

add_executable(3Dviz3 viz_main.cpp viz_mainwindow.cpp
	viz_obj.cpp viz_console.cpp viz_event.cpp viz_surface.cpp
	viz_sphere.cpp viz_triang.cpp
	viz_segments.cpp
	asp_interpolate.c
	asp_gauss.c
	gl2ps.c
	asp_readdir.cpp
	tool_config.cpp
	)

target_link_libraries(3Dviz3 ${OGL} glib-2.0)

