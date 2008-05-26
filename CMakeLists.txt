project(c2)
include(FindJPEG)

add_definitions(-fopenmp)

set(LIBRARY_OUTPUT_PATH "./lib")
set(EXECUTABLE_OUTPUT_PATH "./bin")

set(source main.cpp)
add_executable(c2 ${source})
target_link_libraries(c2 jpeg )

set_target_properties(c2 PROPERTIES  LINK_FLAGS "-fopenmp")

add_subdirectory(fractal)
add_subdirectory(vizualizer)

