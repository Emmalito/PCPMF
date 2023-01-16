#include "gtest/gtest.h"
#include "pnl/pnl_matrix.h"
#include "TestBlackScholesParametersHelper.hpp"
#include "cmath"

using namespace models;

TestBlackScholesParametersHelper::TestBlackScholesParametersHelper()
{
	int und_nb = 3;
	double ir = 0.01;
	double timestep = 2;
	double cor_param = 0.3;
	PnlVect * vol = pnl_vect_create(und_nb);
	LET(vol, 0) = 0.35;
	LET(vol, 1) = 0.45;
	LET(vol, 2) = 0.48;
	bshelper = BlackScholesParametersHelper(und_nb, ir, timestep, cor_param, vol);
}

TEST_F(TestBlackScholesParametersHelper, cholesky_matrix_construction_correct)
{
	PnlMat * under_test = pnl_mat_create(3, 3);
	BlackScholesParametersHelper helper;
	helper.extract_cholesky_corr_matrix(under_test, 0.3);
	ASSERT_NEAR(1, MGET(under_test, 0, 0),0.000001);
	ASSERT_NEAR(0.3, MGET(under_test, 1, 0), 0.000001);
	ASSERT_NEAR(0.953939201, MGET(under_test, 1, 1), 0.000001);
	ASSERT_NEAR(0.3, MGET(under_test, 2, 0), 0.000001);
	ASSERT_NEAR(0.220139816, MGET(under_test, 2, 1), 0.000001);
	ASSERT_NEAR(0.928190962, MGET(under_test, 2, 2), 0.000001);
}

TEST_F(TestBlackScholesParametersHelper, underlying_parameter_constructor_correct)
{
	PnlVect *cholesky_line = pnl_vect_create(4);
	LET(cholesky_line, 0) = 0.3;
	LET(cholesky_line, 1) = 0.1;
	LET(cholesky_line, 2) = 0.15;
	LET(cholesky_line, 3) = 0.32;
	BlackScholesParametersHelper::UnderlyingParameter under_test(cholesky_line, 0.25, 0.11, 0.132);
	EXPECT_EQ(0.25, under_test.sigma_);
	EXPECT_EQ(0.11, under_test.spot_coefficient_);
	EXPECT_EQ(0.132, under_test.sigma_sqrt_timestep_);
	EXPECT_EQ(0.3, GET(under_test.cholesky_line_, 0));
	EXPECT_EQ(0.1, GET(under_test.cholesky_line_, 1));
	EXPECT_EQ(0.15, GET(under_test.cholesky_line_, 2));
	EXPECT_EQ(0.32, GET(under_test.cholesky_line_, 3));
}

TEST_F(TestBlackScholesParametersHelper, constructor_correct)
{
	
	auto underlying_params = bshelper.computed_underlying_parameters();

	BlackScholesParametersHelper::UnderlyingParameter & fst_param = underlying_params[0];
	EXPECT_EQ(0.35, fst_param.sigma_);
	ASSERT_NEAR(-0.1025, fst_param.spot_coefficient_,0.000001);
	ASSERT_NEAR(0.4949747, fst_param.sigma_sqrt_timestep_, 0.000001);
	PnlVect * chol_line = fst_param.cholesky_line_;
	ASSERT_NEAR(1, GET(chol_line, 0), 0.000001);
	ASSERT_NEAR(0, GET(chol_line, 1), 0.000001);
	ASSERT_NEAR(0, GET(chol_line, 2), 0.000001);

	BlackScholesParametersHelper::UnderlyingParameter & scd_param = underlying_params[1];
	EXPECT_EQ(0.45, scd_param.sigma_);
	ASSERT_NEAR(-0.1825, scd_param.spot_coefficient_, 0.000001);
	ASSERT_NEAR(0.6363961, scd_param.sigma_sqrt_timestep_, 0.000001);
	chol_line = scd_param.cholesky_line_;
	ASSERT_NEAR(0.3, GET(chol_line, 0), 0.000001);
	ASSERT_NEAR(0.953939201, GET(chol_line, 1), 0.000001);
	ASSERT_NEAR(0, GET(chol_line, 2), 0.000001);

	BlackScholesParametersHelper::UnderlyingParameter & thrd_param = underlying_params[2];
	EXPECT_EQ(0.48, thrd_param.sigma_);
	ASSERT_NEAR(-0.2104, thrd_param.spot_coefficient_, 0.000001);
	ASSERT_NEAR(0.6788225, thrd_param.sigma_sqrt_timestep_, 0.000001);
	chol_line = thrd_param.cholesky_line_;
	ASSERT_NEAR(0.3, GET(chol_line, 0), 0.000001);
	ASSERT_NEAR(0.220139816, GET(chol_line, 1), 0.000001);
	ASSERT_NEAR(0.928190962, GET(chol_line, 2), 0.000001);
}

TEST_F(TestBlackScholesParametersHelper, asset_simulation_with_precomputed_parameters_correct)
{
	PnlVect * gauss = pnl_vect_create(3);
	LET(gauss, 0) = 0.1;
	LET(gauss, 1) = -0.2;
	LET(gauss, 2) = 0.4;
	
	ASSERT_NEAR(9.48377613, bshelper.compute_one_asset_simulation_with_precomputed_parameters(0, 10, gauss), 0.000001);
	ASSERT_NEAR(7.52146053, bshelper.compute_one_asset_simulation_with_precomputed_parameters(1, 10, gauss), 0.000001);
	ASSERT_NEAR(10.326293, bshelper.compute_one_asset_simulation_with_precomputed_parameters(2, 10, gauss), 0.000001);
}

TEST_F(TestBlackScholesParametersHelper, asset_simulation_correct)
{
	PnlVect * gauss = pnl_vect_create(3);
	LET(gauss, 0) = 0.1;
	LET(gauss, 1) = -0.2;
	LET(gauss, 2) = -0.4;
	double sqrt3 = sqrt(3);
	double result = bshelper.compute_one_asset_simulation(2, 15, 0.02, sqrt3, gauss);
	ASSERT_NEAR(8.18345294, result, 0.000001);
}