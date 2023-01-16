cmake_minimum_required(VERSION 3.14)

project(Options CXX)

set(SOURCE_FILES BarrierOption.cpp)
set(HEADERS BarrierOption.hpp BarrierOptionParameters.hpp)

add_library(options ${SOURCE_FILES} ${HEADERS})