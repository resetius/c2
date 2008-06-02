project(fractal)

set(LIBRARY_OUTPUT_PATH "../lib")
set(EXECUTABLE_OUTPUT_PATH "../bin")

set(source "")

add_flex_files(source c2_lsystem_scanner.l)
add_bison_files(source c2_lsystem_parser.ypp)

add_library(fractal ${source})
add_executable(koch c2_koch.cpp)
target_link_libraries(koch fractal fl)

add_executable(lsystem c2_lsystem.cpp)
target_link_libraries(lsystem fractal fl)

