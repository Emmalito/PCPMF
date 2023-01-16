#include <cmath>
#include "MonteCarloRoutine.hpp"

void MonteCarloRoutine::price(double &price, double &confidence_interval) const
{
	double runningSum = 0;
	double runningSquaredSum = 0;
	double payoff;
	for (unsigned long i = 0; i < sample_number; i++)
	{
		const PnlMat * const generated_path = get_generated_path();

		payoff = option.get_payoff(generated_path);
		runningSum += payoff;
		runningSquaredSum += payoff * payoff;
	}
	double interest_rate = underlying_model.interest_rate();
	double maturity = option.maturity();
	price = exp(-1 * interest_rate * (maturity-t)) * runningSum / sample_number;
	double variance = exp(-2 * interest_rate * maturity) * runningSquaredSum / sample_number - price * price;
	confidence_interval = 1.96 * sqrt(variance / sample_number);
}