project(fractal)

set(LIBRARY_OUTPUT_PATH "../lib")
set(EXECUTABLE_OUTPUT_PATH "../bin")

#if (FFTW3_FOUND)
#	add_definitions("-DHAVE_FFTW")
#endif (FFTW3_FOUND)

set(source c2_lsystem_iter.cpp c2_lines2mgl.cpp c2_lines2txt.cpp c2_lines2png.cpp)
if(WIN32)
set(GD_LIB "")
else(WIN32)
set(GDLIB "gd")
endif(WIN32)

add_flex_files(source c2_lsystem_scanner.l)
add_flex_files(source c2_turtle_scanner.l)
add_bison_files(source c2_lsystem_parser.ypp)

include_directories(../)

add_library(fractal ${source})
add_executable(koch c2_koch.cpp)
target_link_libraries(koch fractal fl)

add_executable(lsystem c2_lsystem.cpp)
target_link_libraries(lsystem fractal fl ${GD_LIB})

add_executable(ifs c2_ifs.cpp)
target_link_libraries(ifs fractal fl ${GD_LIB})

add_executable(mandelbrot c2_mandelbrot.cpp)
target_link_libraries(mandelbrot ${GD_LIB})

add_executable(mandelbulb c2_mandelbulb.cpp)
target_link_libraries(mandelbulb ${GD_LIB})

add_executable(julia c2_julia.cpp)
target_link_libraries(julia ${GD_LIB})

add_executable(fbm_2d c2_fbm.cpp)
target_link_libraries(fbm_2d ${GD_LIB} ${FFTW_LIB})

add_executable(fbm_hcalc c2_fbm_hcalc)
target_link_libraries(fbm_hcalc m)

set_target_properties(mandelbrot PROPERTIES  LINK_FLAGS "-fopenmp")
set_target_properties(mandelbulb PROPERTIES  LINK_FLAGS "-fopenmp")
set_target_properties(julia PROPERTIES  LINK_FLAGS "-fopenmp")

