#pragma once
#include "pnl/pnl_vector.h"

namespace input_parsers
{
	class BlackScholesModelInputParser
	{
	public:
		virtual int get_underlying_number() const = 0;
		virtual int get_monitoring_times() const = 0;
		virtual double get_correlation_parameter() const = 0;
		virtual double get_interest_rate() const = 0;
		virtual double get_final_simulation_date() const = 0;
		virtual PnlVect * get_volatility() const = 0;

		virtual ~BlackScholesModelInputParser() {};
	};
}