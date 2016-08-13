cmake_minimum_required(VERSION 3.5)
project(vmmef)
# add_definitions( ${HDF5_DEFINITIONS} )
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wno-long-long -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# set(CMAKE_MODULE_PATH "${CMAKE_ROOT}/Modules")
set(HDF5_USE_STATIC_LIBRARIES ON)
find_package (HDF5 COMPONENTS CXX)
include_directories(${HDF5_INCLUDE_DIRS})
set(LINK_LIBS ${LINK_LIBS} ${HDF5_LIBRARIES})

set(SOURCE_FILES main.cpp SRSRawFile.cpp SRSRawFile.h FECEvent.cpp FECEvent.h ADCiterator.h)

add_executable(vmmef ${SOURCE_FILES})

target_link_libraries(vmmef ${LINK_LIBS})
