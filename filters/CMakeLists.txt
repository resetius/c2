project(filters)

set(LIBRARY_OUTPUT_PATH "../lib")
set(EXECUTABLE_OUTPUT_PATH "../bin")

set(source main.cpp)
add_executable(c2 ${source})
target_link_libraries(c2 jpeg )

if (OPENMP_FOUND)
set_target_properties(c2 PROPERTIES  LINK_FLAGS "${OpenMP_C_FLAGS}")
endif ()


