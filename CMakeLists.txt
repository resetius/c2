project(c2)
include(FindJPEG)

add_definitions(-fopenmp)

set(source main.cpp)
add_executable(c2 ${source})
target_link_libraries(c2 jpeg gomp)

