project(filters)


set(Boost_USE_MULTITHREADED ON)
find_package(Boost REQUIRED)
set(JPEG_NAMES libjpeg)
find_package(JPEG REQUIRED)

set(LIBRARY_OUTPUT_PATH "../lib")
set(EXECUTABLE_OUTPUT_PATH "../bin")

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${JPEG_INCLUDE_DIR})

set(source main.cpp)
add_executable(c2 ${source})
target_link_libraries(c2 ${Boost_LIBRARIES} ${JPEG_LIBRARIES})

if (OPENMP_FOUND)
set_target_properties(c2 PROPERTIES  LINK_FLAGS "${OpenMP_C_FLAGS}")
endif ()
