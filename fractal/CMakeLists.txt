project(fractal)

set(LIBRARY_OUTPUT_PATH "../lib")
set(EXECUTABLE_OUTPUT_PATH "../bin")

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

find_library(GD_LIB gd NAMES gd bgd bgd_a)

if (GD_LIB-NOTFOUND)
message(FATAL_ERROR "gd library not found")
else ()
message(STATUS "gd library found: ${GD_LIB}")
endif ()

find_path(GD_INCLUDE_DIR "gd.h" HINTS /usr/include ENV INCLUDE)
if (GD_INCLUDE_DIR-NOTFOUND)
message(FATAL_ERROR "gd includes not found")
else ()
message(STATUS "gd includes found: ${GD_INCLUDE_DIR}")
endif ()

include_directories(${GD_INCLUDE_DIR})

#if (FFTW3_FOUND)
#	add_definitions("-DHAVE_FFTW")
#endif (FFTW3_FOUND)

if (NOT WIN32)
set(MATH "m")
else ()
add_definitions(-DYY_NO_UNISTD_H -D_USE_MATH_DEFINES)
endif ()

set(source c2_lsystem_iter.cpp c2_lines2mgl.cpp c2_lines2txt.cpp c2_lines2png.cpp)

flex_target(c2_lsystem_scanner c2_lsystem_scanner.l ${CMAKE_CURRENT_BINARY_DIR}/c2_lsystem_scanner.cpp)
flex_target(c2_turtle_scanner c2_turtle_scanner.l ${CMAKE_CURRENT_BINARY_DIR}/c2_turtle_scanner.cpp)
bison_target(c2_lsystem_parser c2_lsystem_parser.ypp ${CMAKE_CURRENT_BINARY_DIR}/c2_lsystem_parser.cpp)

add_flex_bison_dependency(c2_lsystem_scanner c2_lsystem_parser)
set(source ${source} 
	${BISON_c2_lsystem_parser_OUTPUTS}
	${FLEX_c2_lsystem_scanner_OUTPUTS}
	${FLEX_c2_turtle_scanner_OUTPUTS}
	)

include_directories(../)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(fractal ${source})
add_executable(koch c2_koch.cpp)
target_link_libraries(koch fractal )

add_executable(lsystem c2_lsystem.cpp)
target_link_libraries(lsystem fractal  ${GD_LIB})

add_executable(ifs c2_ifs.cpp)
target_link_libraries(ifs fractal  ${GD_LIB})

add_executable(mandelbrot c2_mandelbrot.cpp)
target_link_libraries(mandelbrot ${GD_LIB})

add_executable(mandelbulb c2_mandelbulb.cpp)
target_link_libraries(mandelbulb ${GD_LIB})

add_executable(julia c2_julia.cpp)
target_link_libraries(julia ${GD_LIB})

add_executable(fbm_2d c2_fbm.cpp)
target_link_libraries(fbm_2d ${GD_LIB} ${FFTW_LIB})

add_executable(fbm_hcalc c2_fbm_hcalc)
target_link_libraries(fbm_hcalc ${MATH})

if (OPENMP_FOUND)
set_target_properties(mandelbrot PROPERTIES  LINK_FLAGS "${OpenMP_C_FLAGS}")
set_target_properties(mandelbulb PROPERTIES  LINK_FLAGS "${OpenMP_C_FLAGS}")
set_target_properties(julia PROPERTIES  LINK_FLAGS "${OpenMP_C_FLAGS}")

set_target_properties(lsystem PROPERTIES  LINK_FLAGS "${OpenMP_C_FLAGS}")
set_target_properties(koch PROPERTIES  LINK_FLAGS "${OpenMP_C_FLAGS}")
set_target_properties(ifs PROPERTIES  LINK_FLAGS "${OpenMP_C_FLAGS}")
set_target_properties(fbm_2d PROPERTIES  LINK_FLAGS "${OpenMP_C_FLAGS}")
set_target_properties(fbm_hcalc PROPERTIES  LINK_FLAGS "${OpenMP_C_FLAGS}")
endif ()
