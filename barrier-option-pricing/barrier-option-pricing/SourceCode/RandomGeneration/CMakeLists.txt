cmake_minimum_required(VERSION 3.14)

project(RandomGeneration CXX)

set(SOURCE_FILES PnlRandomGeneration.cpp)
set(HEADERS RandomGeneration.hpp PnlRandomGeneration.hpp)

add_library(random_generation ${SOURCE_FILES} ${HEADERS})