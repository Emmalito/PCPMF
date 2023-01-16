#include "gtest/gtest.h"
#include "TestBarrierOption.hpp"

#include "pnl/pnl_vector.h"

using namespace options;


TestBarrierOption::TestBarrierOption()
{
}


BarrierOptionParameters TestBarrierOption::setup_option_parameters()
{
	double mat = 4;
	double stk = 2;
	PnlVect * pay_coeff = pnl_vect_create(3);
	LET(pay_coeff, 0) = 1;
	LET(pay_coeff, 1) = 2;
	LET(pay_coeff, 2) = 3;
	PnlVect * up_barrier = pnl_vect_create(3);
	LET(up_barrier, 0) = 3;
	LET(up_barrier, 1) = 3.25;
	LET(up_barrier, 2) = 3.5;
	PnlVect * low_barrier = pnl_vect_create(3);
	LET(low_barrier, 0) = 1;
	LET(low_barrier, 1) = 1.25;
	LET(low_barrier, 2) = 1.5;
	return BarrierOptionParameters(3, mat, stk, pay_coeff, up_barrier, low_barrier);
}

BarrierOption TestBarrierOption::get_barrier_option()
{
	BarrierOptionParameters option_parameters = setup_option_parameters();
	BarrierOption res(option_parameters);
	return res;
}




TEST_F(TestBarrierOption, zero_payoff_when_lower_barrier_is_hit_at_start)
{
	BarrierOptionParameters option_parameters = setup_option_parameters();
	BarrierOption under_test(option_parameters);
	PnlMat *path = pnl_mat_create(3, 3);
	double payoff;
	
	pnl_mat_set_all(path, 2);
	MLET(path, 0, 0) = 0.5;
	payoff = under_test.get_payoff(path);
	EXPECT_EQ(0, payoff);

	pnl_mat_set_all(path, 2);
	MLET(path, 0, 1) = 0.5;
	payoff = under_test.get_payoff(path);
	EXPECT_EQ(0, payoff);

	pnl_mat_set_all(path, 2);
	MLET(path, 0, 2) = 0.5;
	payoff = under_test.get_payoff(path);
	EXPECT_EQ(0, payoff);

	pnl_mat_free(&path);
}

TEST_F(TestBarrierOption, zero_payoff_when_lower_barrier_is_hit_at_middle)
{
	BarrierOptionParameters option_parameters = setup_option_parameters();
	BarrierOption under_test(option_parameters);
	PnlMat *path = pnl_mat_create(3, 3);
	double payoff;

	pnl_mat_set_all(path, 2);
	MLET(path, 1, 0) = 0.5;
	payoff = under_test.get_payoff(path);
	EXPECT_EQ(0, payoff);

	pnl_mat_set_all(path, 2);
	MLET(path, 1, 1) = 0.5;
	payoff = under_test.get_payoff(path);
	EXPECT_EQ(0, payoff);

	pnl_mat_set_all(path, 2);
	MLET(path, 1, 2) = 0.5;
	payoff = under_test.get_payoff(path);
	EXPECT_EQ(0, payoff);

	pnl_mat_free(&path);
}

TEST_F(TestBarrierOption, zero_payoff_when_lower_barrier_is_hit_at_end)
{
	BarrierOptionParameters option_parameters = setup_option_parameters();
	BarrierOption under_test(option_parameters);
	PnlMat *path = pnl_mat_create(3, 3);
	double payoff;

	pnl_mat_set_all(path, 2);
	MLET(path, 2, 0) = 0.5;
	payoff = under_test.get_payoff(path);
	EXPECT_EQ(0, payoff);

	pnl_mat_set_all(path, 2);
	MLET(path, 2, 1) = 0.5;
	payoff = under_test.get_payoff(path);
	EXPECT_EQ(0, payoff);

	pnl_mat_set_all(path, 2);
	MLET(path, 2, 2) = 0.5;
	payoff = under_test.get_payoff(path);
	EXPECT_EQ(0, payoff);

	pnl_mat_free(&path);
}

TEST_F(TestBarrierOption, zero_payoff_when_upper_barrier_is_hit_at_start)
{
	BarrierOptionParameters option_parameters = setup_option_parameters();
	BarrierOption under_test(option_parameters);
	PnlMat *path = pnl_mat_create(3, 3);
	double payoff;

	pnl_mat_set_all(path, 2);
	MLET(path, 0, 0) = 4;
	payoff = under_test.get_payoff(path);
	EXPECT_EQ(0, payoff);

	pnl_mat_set_all(path, 2);
	MLET(path, 0, 1) = 4; 
	payoff = under_test.get_payoff(path);
	EXPECT_EQ(0, payoff);

	pnl_mat_set_all(path, 2);
	MLET(path, 0, 2) = 4;
	payoff = under_test.get_payoff(path);
	EXPECT_EQ(0, payoff);

	pnl_mat_free(&path);
}

TEST_F(TestBarrierOption, zero_payoff_when_upper_barrier_is_hit_at_middle)
{
	BarrierOptionParameters option_parameters = setup_option_parameters();
	BarrierOption under_test(option_parameters);
	PnlMat *path = pnl_mat_create(3, 3);
	double payoff;

	pnl_mat_set_all(path, 2);
	MLET(path, 1, 0) = 4;
	payoff = under_test.get_payoff(path);
	EXPECT_EQ(0, payoff);

	pnl_mat_set_all(path, 2);
	MLET(path, 1, 1) = 4;
	payoff = under_test.get_payoff(path);
	EXPECT_EQ(0, payoff);

	pnl_mat_set_all(path, 2);
	MLET(path, 1, 2) = 4;
	payoff = under_test.get_payoff(path);
	EXPECT_EQ(0, payoff);

	pnl_mat_free(&path);
}

TEST_F(TestBarrierOption, zero_payoff_when_upper_barrier_is_hit_at_end)
{
	BarrierOptionParameters option_parameters = setup_option_parameters();
	BarrierOption under_test(option_parameters);
	PnlMat *path = pnl_mat_create(3, 3);
	double payoff;

	pnl_mat_set_all(path, 2);
	MLET(path, 2, 0) = 4;
	payoff = under_test.get_payoff(path);
	EXPECT_EQ(0, payoff);

	pnl_mat_set_all(path, 2);
	MLET(path, 2, 1) = 4;
	payoff = under_test.get_payoff(path);
	EXPECT_EQ(0, payoff);

	pnl_mat_set_all(path, 2);
	MLET(path, 2, 2) = 4;
	payoff = under_test.get_payoff(path);
	EXPECT_EQ(0, payoff);

	pnl_mat_free(&path);
}

TEST_F(TestBarrierOption, correct_payoff_when_no_barrier_is_hit)
{
	BarrierOptionParameters option_parameters = setup_option_parameters();
	BarrierOption under_test(option_parameters);
	PnlMat *path = pnl_mat_create(3, 3);
	double payoff;

	pnl_mat_set_all(path, 2);
	payoff = under_test.get_payoff(path);
	EXPECT_EQ(10, payoff);
}