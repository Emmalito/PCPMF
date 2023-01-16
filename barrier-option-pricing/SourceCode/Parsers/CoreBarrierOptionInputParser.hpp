#pragma once

#include "BarrierOptionInputParser.hpp"
//#include "Parser.hpp"
#include "parser/include/jlparser/parser.hpp"
//#include "jlparser\parser.hpp"

namespace input_parsers
{
	class CoreBarrierOptionInputParser : public BarrierOptionInputParser
	{
	private:
		const Parser &parser;
		int underlying_number;
	public:
		int get_underlying_number() const;
		double get_maturity() const;
		double get_strike() const;
		PnlVect * get_payoff_coefficients() const;
		PnlVect * get_lower_barrier() const;
		PnlVect * get_upper_barrier() const;
		CoreBarrierOptionInputParser(const Parser &p) : parser(p) 
		{
			parser.extract("option size", underlying_number);
		};
		~CoreBarrierOptionInputParser() {};
	};
}