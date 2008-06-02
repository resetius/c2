project(fractal)

set(LIBRARY_OUTPUT_PATH "../lib")
set(EXECUTABLE_OUTPUT_PATH "../bin")

set(source c2_koch.cpp)

add_flex_files(source c2_lsystem_scanner.l)
add_bison_files(source c2_lsystem.ypp)

add_executable(koch ${source})
target_link_libraries(koch fl)

