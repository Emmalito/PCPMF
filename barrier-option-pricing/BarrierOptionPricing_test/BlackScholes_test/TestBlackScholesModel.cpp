#include "TestBlackScholesModel.hpp"
#include "BlackScholesModel.hpp"
#include "cmath"

using namespace models;

TestBlackScholesModel::TestBlackScholesModel() {};

void TestBlackScholesModel::SetUp() {};

TEST_F(TestBlackScholesModel, constructor_sets_simple_parameters_correctly)
{
	BlackScholesModel under_test(fake_model_parser, fake_random_generator);
	EXPECT_EQ(5, under_test.underlying_number());
	EXPECT_EQ(0.05, under_test.interest_rate());
	EXPECT_EQ(4, under_test.final_simulation_date());	
	EXPECT_EQ(0.5, under_test.timestep());
	EXPECT_EQ(9, under_test.total_historical_path_size());
}

TEST_F(TestBlackScholesModel, simulate_path_unsafe_not_monitoring_date_end_correct)
{
	BlackScholesModel under_test(fake_model_parser, fake_random_generator);
	PnlMat *past_matrix = pnl_mat_create_from_scalar(9, 5, 5);
	const PnlMat * result = under_test.simulate_asset_paths_unsafe(3.75, past_matrix);
	PnlVect * vol = fake_model_parser.get_volatility();
	for (int i = 0; i < under_test.underlying_number(); i++)
	{
		double sigma = GET(vol, i);
		double res = 5 * exp(0.25*(0.05 - 0.5 * sigma * sigma));
		EXPECT_EQ(res, MGET(result, 8, i)); 
	}
	pnl_mat_free(&past_matrix);
}

TEST_F(TestBlackScholesModel, simulate_path_unsafe_at_monitoring_date_end_correct)
{
	BlackScholesModel under_test(fake_model_parser, fake_random_generator);
	PnlMat *past_matrix = pnl_mat_create_from_scalar(9, 5, 5);
	const PnlMat * const result = under_test.simulate_asset_paths_unsafe(4, past_matrix);
	for (int i = 0; i < under_test.underlying_number(); i++)
	{
		EXPECT_EQ(5, MGET(result, 8, i));
	}
	pnl_mat_free(&past_matrix);
}

TEST_F(TestBlackScholesModel, simulate_path_unsafe_from_start_correct)
{
	BlackScholesModel under_test(fake_model_parser, fake_random_generator);
	PnlVect * spots = pnl_vect_create(5);
	for (int i = 0; i < 5; i++)
	{
		LET(spots, i) = 5;
	}
	const PnlMat * const result = under_test.simulate_asset_paths_from_start(spots);
	PnlVect * vol = fake_model_parser.get_volatility();
	for (int j = 0; j < 5; j++)
	{
		double sigma = GET(vol, j);
		for (int i = 0; i < 9; i++)
		{
			double t = 0.5 * i;
			double res = 5 * exp(t*(0.05 - 0.5 * sigma * sigma));
			ASSERT_NEAR(res, MGET(result, i, j), 0.000001);
		}
	}
	pnl_vect_free(&spots);
}

TEST_F(TestBlackScholesModel, simulate_path_unsafe_at_start_monitoring_date_gives_same_value)
{
	BlackScholesModel under_test(fake_model_parser, fake_random_generator);
	PnlVect * spots = pnl_vect_create(5);
	for (int i = 0; i < 5; i++)
	{
		LET(spots, i) = 5;
	}
	PnlMat *past_matrix = pnl_mat_create_from_scalar(1, 5, 5);
	const PnlMat * const monitor_result = under_test.simulate_asset_paths_unsafe(0, past_matrix);
	const PnlMat * const from_start_result = under_test.simulate_asset_paths_from_start(spots);
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			EXPECT_EQ(MGET(monitor_result, i, j), MGET(from_start_result, i, j));
		}
	}
	pnl_vect_free(&spots);
	pnl_mat_free(&past_matrix);
}