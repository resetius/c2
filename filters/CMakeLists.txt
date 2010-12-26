project(filters)

set(Boost_USE_MULTITHREADED ON)
find_package(Boost COMPONENTS gil)

set(LIBRARY_OUTPUT_PATH "../lib")
set(EXECUTABLE_OUTPUT_PATH "../bin")

if(Boost_FOUND)
include_directories(${Boost_INCLUDE_DIRS})

set(source main.cpp)
add_executable(c2 ${source})
target_link_libraries(c2 jpeg ${Boost_LIBRARIES})

if (OPENMP_FOUND)
set_target_properties(c2 PROPERTIES  LINK_FLAGS "${OpenMP_C_FLAGS}")
endif ()

endif ()
