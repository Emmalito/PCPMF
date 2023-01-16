cmake_minimum_required(VERSION 3.14)
#TODO: in cmake-gui, add CMAKE_PREFIX_PATH, set it to the pnl directory (that contains lib and include foleders)
project(Project_BarrierOptionPricing CXX)


set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if (MSVC_VERSION GREATER_EQUAL "1900")
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
    if (_cpp_latest_flag_supported)
        add_compile_options("/std:c++latest")
    endif()
endif()

# Detect PNL
find_package(Pnl REQUIRED)
set(LIBS ${LIBS} ${PNL_LIBRARIES})
include_directories(${PNL_INCLUDE_DIRS})

include_directories(
	SourceCode/Models 
	SourceCode/Options 
	SourceCode/Parsers 
	SourceCode/RandomGeneration 
	SourceCode/BarrierOptionPricing 
	SourceCode/Infrastructure 
	${PNL_INCLUDE} 
)

add_subdirectory(SourceCode/Models)
add_subdirectory(SourceCode/Options)
add_subdirectory(SourceCode/Parsers)
add_subdirectory(SourceCode/RandomGeneration)
add_subdirectory(SourceCode/BarrierOptionPricing)
add_subdirectory(BarrierOptionPricing_test)

