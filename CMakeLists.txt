cmake_minimum_required(VERSION 3.5)
include(CheckCXXCompilerFlag)
include(FeatureSummary)

project(vmmef CXX)

check_cxx_compiler_flag(-Wall HAS_CXX_PEDANTIC)             # C++ Flags
if (HAS_CXX_PEDANTIC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic ")
endif()
check_cxx_compiler_flag(-Wall HAS_CXX_WALL)
if (HAS_CXX_WALL)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall ")
endif()
check_cxx_compiler_flag(-std=c++11 HAS_CXX_CXX11)
if(HAS_CXX_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ")
else()
    message( FATAL_ERROR "Please use a compiler that supports C++11" )
endif()

find_package (HDF5 REQUIRED COMPONENTS CXX)
if (HDF5_FOUND)
    include_directories(${HDF5_INCLUDE_DIRS})
    set(LINK_LIBS ${LINK_LIBS} ${HDF5_LIBRARIES})
endif()

set(SOURCE_FILES main.cpp SRSRawFile.cpp SRSRawFile.h FECEvent.cpp FECEvent.h ADCiterator.h)

add_executable(vmmef ${SOURCE_FILES})
target_link_libraries(vmmef ${LINK_LIBS})
