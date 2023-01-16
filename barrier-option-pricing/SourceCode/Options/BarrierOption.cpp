#include "BarrierOption.hpp"
#include <algorithm>
using namespace options;

double BarrierOption::get_payoff(const PnlMat * const underlying_paths) const
{
	int number_of_values = underlying_paths->m;
	int underlying_number = parameters.underlying_number;
	double strike = parameters.strike;
	PnlVect * lower_barrier = parameters.lower_barrier;
	PnlVect * upper_barrier = parameters.upper_barrier;
	PnlVect * payoff_coefficients = parameters.payoff_coefficients;
	for (int i = 0; i < number_of_values; i++)
	{
		for (int j = 0; j < underlying_number; j++)
		{
			double current_value = MGET(underlying_paths, i, j);
			if (current_value < GET(lower_barrier, j) || current_value > GET(upper_barrier, j))
			{
				return 0;
			}
		}
	}
	double weighted_sum = 0;
	int last_value_index = number_of_values - 1;
	for (int j = 0; j < underlying_number; j++)
	{
		weighted_sum += MGET(underlying_paths, last_value_index, j) * GET(payoff_coefficients, j);
	}
	double payoff = std::max(weighted_sum - strike, 0.0);
	return payoff;
}
