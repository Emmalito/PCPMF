cmake_minimum_required(VERSION 3.14)

project(BarrierOptionPricing_test)

if (MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")    
endif()

include(FetchContent)
FetchContent_Declare(
  	googletest
  	GIT_REPOSITORY https://github.com/google/googletest.git
  	GIT_TAG release-1.12.1
)
if (MSVC)
	# For Windows: Prevent overriding the parent project's compiler/linker settings
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()
FetchContent_MakeAvailable(googletest)
		
include_directories(Fakes BarrierOption_test BlackScholes_test)

set(HEADERS 
	BarrierOption_test/TestBarrierOptionParameters.hpp 
	BarrierOption_test/TestBarrierOption.hpp 
	BlackScholes_test/TestBlackScholesModel.hpp  
	BlackScholes_test/TestBlackScholesParametersHelper.hpp
)

set(TEST_FILES 
	BarrierOption_test/TestBarrierOption.cpp 
	BarrierOption_test/TestBarrierOptionParameters.cpp
	BlackScholes_test/TestBlackScholesModel.cpp  
	BlackScholes_test/TestBlackScholesParametersHelper.cpp
)

enable_testing()


add_executable(run_unit_tests BarrierOptionPricing_test.cpp ${HEADERS} ${TEST_FILES} $<TARGET_OBJECTS:gtest>)

target_link_libraries(run_unit_tests gtest_main options models ${LIBS})
# If the code does not compile, try commenting the 'target_link_libraries' line above and uncommenting the one below.
#target_link_libraries(run_unit_tests -lpthread -lm)

add_test(run_unit_tests run_unit_tests)
pnl_add_postbuild(run_unit_tests)


