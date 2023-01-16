cmake_minimum_required(VERSION 3.14)

project(Parsers CXX)

set(SOURCE_FILES CoreBarrierOptionInputParser.cpp CoreBlackScholesModelInputParser.cpp)
set(HEADERS BarrierOptionInputParser.hpp BlackScholesModelInputParser.hpp CoreBarrierOptionInputParser.hpp CoreBlackScholesModelInputParser.hpp)

add_subdirectory(parser)
add_library(parsers ${SOURCE_FILES} ${HEADERS})
target_link_libraries(parsers jl_parser)