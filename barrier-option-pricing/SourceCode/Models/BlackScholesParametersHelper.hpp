#pragma once
#include <vector>
#include "pnl/pnl_vector.h"

namespace models
{

	class BlackScholesParametersHelper
	{
	public:
		struct UnderlyingParameter
		{
			PnlVect * cholesky_line_;
			double sigma_;
			double spot_coefficient_;
			double sigma_sqrt_timestep_;

			UnderlyingParameter() : sigma_(0) {};

			UnderlyingParameter(PnlVect *cholesky_line, double sigma, double spot_coefficient, double sigma_sqrt_timestep) :
				sigma_(sigma),
				spot_coefficient_(spot_coefficient),
				sigma_sqrt_timestep_(sigma_sqrt_timestep)
			{
				cholesky_line_ = pnl_vect_copy(cholesky_line);
			};

			UnderlyingParameter(const UnderlyingParameter &other) :
				sigma_(other.sigma_),
				spot_coefficient_(other.spot_coefficient_),
				sigma_sqrt_timestep_(other.sigma_sqrt_timestep_)
			{
				cholesky_line_ = pnl_vect_copy(other.cholesky_line_);
			}

			UnderlyingParameter & operator=(UnderlyingParameter && other)
			{
				if (this != &other)
				{
					sigma_ = other.sigma_;
					spot_coefficient_ = other.spot_coefficient_;
					sigma_sqrt_timestep_ = other.sigma_sqrt_timestep_;
					cholesky_line_ = other.cholesky_line_;
					other.cholesky_line_ = nullptr;
				}
				return *this;
			}

			~UnderlyingParameter() { pnl_vect_free(&cholesky_line_); };
		};

		BlackScholesParametersHelper() {};
		BlackScholesParametersHelper(const int underlying_number, const double interest_rate, const double timestep, const double correlation_parameter, const PnlVect * const volatilities);
		std::vector<UnderlyingParameter> computed_underlying_parameters() { return computed_underlying_parameters_; }

		double compute_one_asset_simulation(const int underlying_position, const double spot, const double interest_rate, const double sqrt_timelength, const PnlVect * const gaussian_vector) const;
		double compute_one_asset_simulation_with_precomputed_parameters(const int underlying_position, const double spot, const PnlVect * const gaussian_vector) const;
		void extract_cholesky_corr_matrix(PnlMat *correlation_matrix, double correlation_parameter) const;

	private:
		std::vector<UnderlyingParameter> computed_underlying_parameters_;
		double asset_simulation_helper(const UnderlyingParameter & underlying_parameter, const double spot, const PnlVect * const gaussian_vector) const;
	};

}