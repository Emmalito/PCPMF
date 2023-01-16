#pragma once
#include "gtest/gtest.h"
#include "BlackScholesParametersHelper.hpp"

class TestBlackScholesParametersHelper : public testing::Test
{
public:
	models::BlackScholesParametersHelper bshelper;
	TestBlackScholesParametersHelper();
};

