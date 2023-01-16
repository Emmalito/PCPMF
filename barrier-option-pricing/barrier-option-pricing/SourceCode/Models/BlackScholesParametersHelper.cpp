#include <cmath>
#include "pnl/pnl_matrix.h"
#include "BlackScholesParametersHelper.hpp"

using namespace models;
using namespace std;

BlackScholesParametersHelper::BlackScholesParametersHelper(const int underlying_number, const double interest_rate, const double timestep, const double correlation_parameter, const PnlVect * const volatilities)
{
	PnlMat *correlation_matrix = pnl_mat_create(underlying_number, underlying_number);
	extract_cholesky_corr_matrix(correlation_matrix, correlation_parameter);
	double sqrt_timestep = sqrt(timestep);
	for (int i = 0; i < underlying_number; i++)
	{
		PnlVect *cholesky_line = pnl_vect_create(underlying_number);
		pnl_mat_get_row(cholesky_line, correlation_matrix, i);
		double sigma_i = GET(volatilities, i);
		double spot_coefficient = (interest_rate - 0.5 * sigma_i * sigma_i) * timestep;
		double sigma_i_sqrt_timestep = sigma_i * sqrt_timestep;

		UnderlyingParameter computed_up(cholesky_line, sigma_i, spot_coefficient, sigma_i_sqrt_timestep);
		computed_underlying_parameters_.push_back(computed_up);
	}
}

void BlackScholesParametersHelper::extract_cholesky_corr_matrix(PnlMat *correlation_matrix, double correlation_parameter) const
{
	int underlying_number = correlation_matrix->m;
	for (int i = 0; i < underlying_number; i++)
	{
		for (int j = 0; j < underlying_number; j++)
		{
			if (i != j)
			{
				MLET(correlation_matrix, i, j) = correlation_parameter;
			}
			else
			{
				MLET(correlation_matrix, i, j) = 1;
			}
		}
	}
	pnl_mat_chol(correlation_matrix);
}

double BlackScholesParametersHelper::compute_one_asset_simulation(const int underlying_position, const double spot, const double interest_rate, const double sqrt_timelength, const PnlVect * const gaussian_vector) const
{
	BlackScholesParametersHelper::UnderlyingParameter tmp_underlying_parameter(computed_underlying_parameters_[underlying_position]);
	double sigma_i = tmp_underlying_parameter.sigma_;
	tmp_underlying_parameter.spot_coefficient_ = (interest_rate - 0.5 * sigma_i * sigma_i) * sqrt_timelength * sqrt_timelength;
	tmp_underlying_parameter.sigma_sqrt_timestep_ = sigma_i * sqrt_timelength;

	return asset_simulation_helper(tmp_underlying_parameter, spot, gaussian_vector);
}

double BlackScholesParametersHelper::compute_one_asset_simulation_with_precomputed_parameters(const int underlying_position, const double spot, const PnlVect * const gaussian_vector) const
{
	return asset_simulation_helper(computed_underlying_parameters_[underlying_position], spot, gaussian_vector);
}

double BlackScholesParametersHelper::asset_simulation_helper(const UnderlyingParameter & underlying_parameter, const double spot, const PnlVect * const gaussian_vector) const
{
	double moved_spot = spot * exp(underlying_parameter.spot_coefficient_);
	const PnlVect * const cholesky_line = underlying_parameter.cholesky_line_;
	double time_factor = underlying_parameter.sigma_sqrt_timestep_;
	double scalar_product = pnl_vect_scalar_prod(cholesky_line, gaussian_vector);
	double gaussian_factor = exp(time_factor * scalar_product);

	return moved_spot * gaussian_factor;
}

