#pragma once
#include "pnl/pnl_vector.h"

namespace input_parsers
{

	class BarrierOptionInputParser
	{
	public:
		virtual int get_underlying_number() const = 0;
		virtual double get_maturity() const = 0;
		virtual double get_strike() const = 0;
		virtual PnlVect * get_payoff_coefficients() const = 0;
		virtual PnlVect * get_lower_barrier() const = 0;
		virtual PnlVect * get_upper_barrier() const = 0;	
		virtual ~BarrierOptionInputParser() {};
	};

}