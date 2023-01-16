#pragma once
#include <utility>
#include <iostream>
#include "pnl/pnl_matrix.h"
#include "pnl/pnl_vector.h"
#include "CustomExceptions.hpp"
#include "BarrierOptionParameters.hpp"

namespace options {
	class BarrierOption
	{
	private:
		BarrierOptionParameters& parameters;

	public:
		BarrierOption(BarrierOptionParameters &params) :
			parameters(params)
		{};

		double maturity() const { return parameters.maturity; }
		double strike() const { return parameters.strike; }
		int underlying_number() const { return parameters.underlying_number; }
		double get_payoff(const PnlMat * const underlying_paths) const;
		const PnlVect * const lower_barrier() const { return parameters.lower_barrier; }
		const PnlVect * const upper_barrier() const { return parameters.upper_barrier; }
		const PnlVect * const payoff_coefficients() const { return parameters.payoff_coefficients; }
	};

}