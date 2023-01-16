cmake_minimum_required(VERSION 3.2)
project(JLParser CXX)

# Release or Debug
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif ()

set(CMAKE_CXX_STANDARD 17) # C++11...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11

add_library(jl_parser INTERFACE)
target_include_directories(jl_parser INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_sources(jl_parser INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/parser.cpp)

# Detect PNL
find_package(Pnl REQUIRED)
set(LIBS ${LIBS} ${PNL_LIBRARIES})
include_directories(${PNL_INCLUDE_DIRS})
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_definitions(-DPNL_RANGE_CHECK_OFF)
endif()

# Add -lm -ldl if needed
if (UNIX)
    set(LIBS ${LIBS} m dl)
endif (UNIX)

# Testing the parser
add_executable(parser-test test_parser.cpp)
target_link_libraries(parser-test ${LIBS} jl_parser)