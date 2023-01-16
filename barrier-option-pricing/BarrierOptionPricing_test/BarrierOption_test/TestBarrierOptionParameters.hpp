#pragma once
#include "gtest/gtest.h"
#include "FakeBarrierOptionInputParser.hpp"

class TestBarrierOptionParameters : public testing::Test
{
public:
	virtual void SetUp();
	TestBarrierOptionParameters();	
	FakeBarrierOptionInputParser fake_input_parser;
};

