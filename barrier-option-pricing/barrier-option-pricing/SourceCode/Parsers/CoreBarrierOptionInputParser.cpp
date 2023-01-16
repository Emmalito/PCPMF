#include "CoreBarrierOptionInputParser.hpp"

using namespace input_parsers;

int CoreBarrierOptionInputParser::get_underlying_number() const
{
	return underlying_number;
}

double CoreBarrierOptionInputParser::get_maturity() const
{
	double result;
	parser.extract("maturity", result);
	return result;
}

double CoreBarrierOptionInputParser::get_strike() const
{
	double result;
	parser.extract("strike", result);
	return result;
}

PnlVect * CoreBarrierOptionInputParser::get_payoff_coefficients() const
{
	PnlVect * result;
	parser.extract("payoff coefficients", result, underlying_number);
	return result;
}

PnlVect * CoreBarrierOptionInputParser::get_lower_barrier() const
{
	PnlVect * result;
	parser.extract("lower barrier", result, underlying_number);
	return result;
}

PnlVect * CoreBarrierOptionInputParser::get_upper_barrier() const
{
	PnlVect * result;
	parser.extract("upper barrier", result, underlying_number);
	return result;
}