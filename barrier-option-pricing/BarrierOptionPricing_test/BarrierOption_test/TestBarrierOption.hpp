#pragma once
#include "gtest/gtest.h"
#include "BarrierOption.hpp"
#include "FakeUnderlyingModel.hpp"
#include "BarrierOptionParameters.hpp"
//#include "boost/shared_ptr.hpp"

class TestBarrierOption : public testing::Test
{
public:
	TestBarrierOption();
	options::BarrierOption get_barrier_option();
	options::BarrierOptionParameters setup_option_parameters();
	

private:
	
	
};

