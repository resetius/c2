
if (NOT FFTW3_QUITE)
set(FFTW3_LIB "")
set(FFTW3_FOUND False)
set(FFTW3_QUITE True)

find_path(FFTW3_INCLUDE fftw3.h
	PATHS "/usr/include" "/usr/local/include"
	)

if (FFTW3_INCLUDE)
	message(STATUS "Found fftw3: ${FFTW3_INCLUDE}")
	set(FFTW3_FOUND True)
	set(FFTW3_LIB "fftw3")
else(FFTW3_INCLUDE)
	message(STATUS "fftw3 not found")
endif(FFTW3_INCLUDE)
endif(NOT FFTW3_QUITE)

