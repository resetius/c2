project(c2)
include(FindJPEG)
include(CheckIncludeFile)
check_include_file(boost/gil/gil_all.hpp BOOST_GIL_EXISTS)


if (defined ${BOOST_GIL_EXISTS})
else (defined ${BOOST_GIL_EXISTS})
	message(FATAL_ERROR "boost::gil not found " ${BOOST_GIL_EXISTS})
endif(defined ${BOOST_GIL_EXISTS})

add_definitions(-fopenmp)

set(source main.cpp)
add_executable(c2 ${source})
target_link_libraries(c2 jpeg gomp)

