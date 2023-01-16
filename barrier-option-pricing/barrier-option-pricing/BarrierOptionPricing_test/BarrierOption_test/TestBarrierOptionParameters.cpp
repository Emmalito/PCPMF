#include "gtest/gtest.h"
#include "TestBarrierOptionParameters.hpp"
#include "BarrierOptionParameters.hpp"

using namespace options;

TestBarrierOptionParameters::TestBarrierOptionParameters() {};

void TestBarrierOptionParameters::SetUp() {};

TEST_F(TestBarrierOptionParameters, constructor_with_input_parser_correct)
{
	BarrierOptionParameters under_test(fake_input_parser);
	
	EXPECT_EQ(3, under_test.underlying_number);
	EXPECT_EQ(2, under_test.maturity);
	EXPECT_EQ(2, under_test.strike);

	const PnlVect * const l_barrier = under_test.lower_barrier;
	EXPECT_EQ(3, l_barrier->size);
	EXPECT_EQ(1, GET(l_barrier, 0));
	EXPECT_EQ(1.25, GET(l_barrier, 1));
	EXPECT_EQ(1.5, GET(l_barrier, 2));

	const PnlVect * const u_barrier = under_test.upper_barrier;
	EXPECT_EQ(3, u_barrier->size);
	EXPECT_EQ(3, GET(u_barrier, 0));
	EXPECT_EQ(3.25, GET(u_barrier, 1));
	EXPECT_EQ(3.5, GET(u_barrier, 2));

	const PnlVect * const p_coeff = under_test.payoff_coefficients;
	EXPECT_EQ(3, p_coeff->size);
	EXPECT_EQ(1, GET(p_coeff, 0));
	EXPECT_EQ(2, GET(p_coeff, 1));
	EXPECT_EQ(3, GET(p_coeff, 2));
	
}

TEST_F(TestBarrierOptionParameters, constructor_with_fields_correct)
{
	int nbu = 3;
	double mat = 2;
	double stk = 2;
	PnlVect * pay_coeff = pnl_vect_create(nbu);
	LET(pay_coeff, 0) = 1;
	LET(pay_coeff, 1) = 2;
	LET(pay_coeff, 2) = 3;
	PnlVect * up_barrier = pnl_vect_create(nbu);
	LET(up_barrier, 0) = 3;
	LET(up_barrier, 1) = 3.25;
	LET(up_barrier, 2) = 3.5;
	PnlVect * low_barrier = pnl_vect_create(nbu);
	LET(low_barrier, 0) = 1;
	LET(low_barrier, 1) = 1.25;
	LET(low_barrier, 2) = 1.5;
	BarrierOptionParameters under_test(nbu, mat, stk, pay_coeff, up_barrier, low_barrier);

	EXPECT_EQ(3, under_test.underlying_number);
	EXPECT_EQ(2, under_test.maturity);
	EXPECT_EQ(2, under_test.strike);

	const PnlVect * const l_barrier = under_test.lower_barrier;
	EXPECT_EQ(3, l_barrier->size);
	EXPECT_EQ(1, GET(l_barrier, 0));
	EXPECT_EQ(1.25, GET(l_barrier, 1));
	EXPECT_EQ(1.5, GET(l_barrier, 2));

	const PnlVect * const u_barrier = under_test.upper_barrier;
	EXPECT_EQ(3, u_barrier->size);
	EXPECT_EQ(3, GET(u_barrier, 0));
	EXPECT_EQ(3.25, GET(u_barrier, 1));
	EXPECT_EQ(3.5, GET(u_barrier, 2));

	const PnlVect * const p_coeff = under_test.payoff_coefficients;
	EXPECT_EQ(3, p_coeff->size);
	EXPECT_EQ(1, GET(p_coeff, 0));
	EXPECT_EQ(2, GET(p_coeff, 1));
	EXPECT_EQ(3, GET(p_coeff, 2));
}

TEST_F(TestBarrierOptionParameters, copy_constructor_correct)
{
	BarrierOptionParameters first_param(fake_input_parser);
	BarrierOptionParameters under_test(first_param);
	EXPECT_EQ(3, under_test.underlying_number);
	EXPECT_EQ(2, under_test.maturity);
	EXPECT_EQ(2, under_test.strike);

	const PnlVect * const l_barrier = under_test.lower_barrier;
	EXPECT_EQ(3, l_barrier->size);
	EXPECT_EQ(1, GET(l_barrier, 0));
	EXPECT_EQ(1.25, GET(l_barrier, 1));
	EXPECT_EQ(1.5, GET(l_barrier, 2));

	const PnlVect * const u_barrier = under_test.upper_barrier;
	EXPECT_EQ(3, u_barrier->size);
	EXPECT_EQ(3, GET(u_barrier, 0));
	EXPECT_EQ(3.25, GET(u_barrier, 1));
	EXPECT_EQ(3.5, GET(u_barrier, 2));

	const PnlVect * const p_coeff = under_test.payoff_coefficients;
	EXPECT_EQ(3, p_coeff->size);
	EXPECT_EQ(1, GET(p_coeff, 0));
	EXPECT_EQ(2, GET(p_coeff, 1));
	EXPECT_EQ(3, GET(p_coeff, 2));
}

TEST_F(TestBarrierOptionParameters, assignment_operator_correct)
{
	BarrierOptionParameters first_param(fake_input_parser);
	BarrierOptionParameters under_test;
	under_test = first_param;
	EXPECT_EQ(3, under_test.underlying_number);
	EXPECT_EQ(2, under_test.maturity);
	EXPECT_EQ(2, under_test.strike);

	const PnlVect * const l_barrier = under_test.lower_barrier;
	EXPECT_EQ(3, l_barrier->size);
	EXPECT_EQ(1, GET(l_barrier, 0));
	EXPECT_EQ(1.25, GET(l_barrier, 1));
	EXPECT_EQ(1.5, GET(l_barrier, 2));

	const PnlVect * const u_barrier = under_test.upper_barrier;
	EXPECT_EQ(3, u_barrier->size);
	EXPECT_EQ(3, GET(u_barrier, 0));
	EXPECT_EQ(3.25, GET(u_barrier, 1));
	EXPECT_EQ(3.5, GET(u_barrier, 2));

	const PnlVect * const p_coeff = under_test.payoff_coefficients;
	EXPECT_EQ(3, p_coeff->size);
	EXPECT_EQ(1, GET(p_coeff, 0));
	EXPECT_EQ(2, GET(p_coeff, 1));
	EXPECT_EQ(3, GET(p_coeff, 2));
}

TEST_F(TestBarrierOptionParameters, move_assignment_operator_correct)
{
	BarrierOptionParameters under_test;
	under_test = BarrierOptionParameters(fake_input_parser);
	EXPECT_EQ(3, under_test.underlying_number);
	EXPECT_EQ(2, under_test.maturity);
	EXPECT_EQ(2, under_test.strike);

	const PnlVect * const l_barrier = under_test.lower_barrier;
	EXPECT_EQ(3, l_barrier->size);
	EXPECT_EQ(1, GET(l_barrier, 0));
	EXPECT_EQ(1.25, GET(l_barrier, 1));
	EXPECT_EQ(1.5, GET(l_barrier, 2));

	const PnlVect * const u_barrier = under_test.upper_barrier;
	EXPECT_EQ(3, u_barrier->size);
	EXPECT_EQ(3, GET(u_barrier, 0));
	EXPECT_EQ(3.25, GET(u_barrier, 1));
	EXPECT_EQ(3.5, GET(u_barrier, 2));

	const PnlVect * const p_coeff = under_test.payoff_coefficients;
	EXPECT_EQ(3, p_coeff->size);
	EXPECT_EQ(1, GET(p_coeff, 0));
	EXPECT_EQ(2, GET(p_coeff, 1));
	EXPECT_EQ(3, GET(p_coeff, 2));
}

TEST_F(TestBarrierOptionParameters, move_constructor_correct)
{
	BarrierOptionParameters first_param(fake_input_parser);
	BarrierOptionParameters under_test(std::move(first_param));
	EXPECT_EQ(3, under_test.underlying_number);
	EXPECT_EQ(2, under_test.maturity);
	EXPECT_EQ(2, under_test.strike);

	const PnlVect * const l_barrier = under_test.lower_barrier;
	EXPECT_EQ(3, l_barrier->size);
	EXPECT_EQ(1, GET(l_barrier, 0));
	EXPECT_EQ(1.25, GET(l_barrier, 1));
	EXPECT_EQ(1.5, GET(l_barrier, 2));

	const PnlVect * const u_barrier = under_test.upper_barrier;
	EXPECT_EQ(3, u_barrier->size);
	EXPECT_EQ(3, GET(u_barrier, 0));
	EXPECT_EQ(3.25, GET(u_barrier, 1));
	EXPECT_EQ(3.5, GET(u_barrier, 2));

	const PnlVect * const p_coeff = under_test.payoff_coefficients;
	EXPECT_EQ(3, p_coeff->size);
	EXPECT_EQ(1, GET(p_coeff, 0));
	EXPECT_EQ(2, GET(p_coeff, 1));
	EXPECT_EQ(3, GET(p_coeff, 2));
}