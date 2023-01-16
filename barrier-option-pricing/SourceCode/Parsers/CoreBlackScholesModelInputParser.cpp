#include "CoreBlackScholesModelInputParser.hpp"

using namespace input_parsers;

int CoreBlackScholesModelInputParser::get_underlying_number() const
{
	return underlying_number;
}

int CoreBlackScholesModelInputParser::get_monitoring_times() const
{
	int result;
	parser.extract("timestep number", result);
	return result;
}

double CoreBlackScholesModelInputParser::get_correlation_parameter() const
{
	double result;
	parser.extract("correlation", result);
	return result;
}

double CoreBlackScholesModelInputParser::get_interest_rate() const
{
	double result;
	parser.extract("interest rate", result);
	return result;
}

double CoreBlackScholesModelInputParser::get_final_simulation_date() const
{
	double result;
	parser.extract("maturity", result); 
	return result;
}

PnlVect * CoreBlackScholesModelInputParser::get_volatility() const
{
	PnlVect * result;
	parser.extract("volatility", result, underlying_number);
	return result;
}