cmake_minimum_required(VERSION 3.5)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
include(CheckCXXCompilerFlag)
include(FeatureSummary)

project(vmmef CXX C)

check_cxx_compiler_flag(-Wall HAS_CXX_WALL)
if (HAS_CXX_WALL)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall ")
endif()
check_cxx_compiler_flag(-pedantic HAS_CXX_PEDANTIC)             # C++ Flags
if (HAS_CXX_PEDANTIC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic ")
endif()
check_cxx_compiler_flag(-std=c++11 HAS_CXX_CXX11)
if(HAS_CXX_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ")
else()
    message( FATAL_ERROR "Please use a compiler that supports C++11" )
endif()

find_package (lmfit)
if (LMFIT_FOUND)
    include_directories(${LMFIT_INCLUDE_DIRS})
    set(LINK_LIBS ${LINK_LIBS} ${LMFIT_LIBRARIES})
else()
    message( FATAL_ERROR "lmfit not found. Please install from http://apps.jcns.fz-juelich.de/doku/sc/lmfit" )
endif()


find_package (HDF5 REQUIRED COMPONENTS CXX)
if (HDF5_FOUND)
    include_directories(${HDF5_INCLUDE_DIRS})
    set(LINK_LIBS ${LINK_LIBS} ${HDF5_LIBRARIES})
endif()

set(SOURCE_FILES vmmef.cpp SRSRawFile.cpp SRSRawFile.h FECEvent.cpp FECEvent.h ADCiterator.h Pedestal.cpp Pedestal.h lmng.h lmng.c vmmef.h Histogram.cpp Histogram.h HDF5File.cpp HDF5File.h EventImage.cpp EventImage.h)

add_executable(vmmef ${SOURCE_FILES})
target_link_libraries(vmmef ${LINK_LIBS})
