#pragma once
#include "gtest/gtest.h"
#include "ConstantRandomGeneration.hpp"
#include "FakeBlackScholesModelInputParser.hpp"

class TestBlackScholesModel : public testing::Test
{
public:
	virtual void SetUp();
	TestBlackScholesModel();
	ConstantRandomGeneration fake_random_generator;
	FakeBlackScholesModelInputParser fake_model_parser;
};