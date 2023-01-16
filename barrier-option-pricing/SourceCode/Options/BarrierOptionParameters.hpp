#pragma once
#include <utility>
#include <iostream>
#include "pnl/pnl_vector.h"
#include "UnderlyingModel.hpp"
#include "BarrierOptionInputParser.hpp"

namespace options {

	struct BarrierOptionParameters
	{
		int underlying_number; 
		double maturity;
		double strike;
		PnlVect * payoff_coefficients; 
		PnlVect * upper_barrier; 
		PnlVect * lower_barrier;

		BarrierOptionParameters() :
			underlying_number(0),
			maturity(0),
			strike(0),
			payoff_coefficients(nullptr),
			upper_barrier(nullptr),
			lower_barrier(nullptr)
		{};

		BarrierOptionParameters(const input_parsers::BarrierOptionInputParser &input_parser) 
		{
			underlying_number = input_parser.get_underlying_number();
			maturity = input_parser.get_maturity();
			strike = input_parser.get_strike();
			payoff_coefficients = pnl_vect_copy(input_parser.get_payoff_coefficients());
			lower_barrier = pnl_vect_copy(input_parser.get_lower_barrier());
			upper_barrier = pnl_vect_copy(input_parser.get_upper_barrier());
		};

		BarrierOptionParameters(int underlyings, double mat, double stk, PnlVect * payoff_coeff, PnlVect * up_barrier, PnlVect * low_barrier) :
			underlying_number(underlyings),
			maturity(mat),
			strike(stk)
		{
			payoff_coefficients = pnl_vect_copy(payoff_coeff);
			upper_barrier = pnl_vect_copy(up_barrier);
			lower_barrier = pnl_vect_copy(low_barrier);
		};


		BarrierOptionParameters(const BarrierOptionParameters & other) :
			underlying_number(other.underlying_number),
			maturity(other.maturity),
			strike(other.strike)
		{
			payoff_coefficients = pnl_vect_copy(other.payoff_coefficients);
			upper_barrier = pnl_vect_copy(other.upper_barrier);
			lower_barrier = pnl_vect_copy(other.lower_barrier);
		}

		BarrierOptionParameters(BarrierOptionParameters && other) :
			underlying_number(other.underlying_number),
			maturity(other.maturity),
			strike(other.strike),
			payoff_coefficients(other.payoff_coefficients),
			upper_barrier(other.upper_barrier),
			lower_barrier(other.lower_barrier)
		{
			other.payoff_coefficients = nullptr;
			other.upper_barrier = nullptr;
			other.lower_barrier = nullptr;
		}

		BarrierOptionParameters& operator=(BarrierOptionParameters && other) 
		{
			if (this != &other)
			{
				underlying_number = other.underlying_number;
				maturity = other.maturity;
				strike = other.strike;
				payoff_coefficients = other.payoff_coefficients;
				upper_barrier = other.upper_barrier;
				lower_barrier = other.lower_barrier;
				other.payoff_coefficients = nullptr;
				other.upper_barrier = nullptr;
				other.lower_barrier = nullptr;
			}
			return *this;
		}


		BarrierOptionParameters& operator=(const BarrierOptionParameters & other)
		{
			if (this != &other)
			{
				underlying_number = other.underlying_number;
				maturity = other.maturity;
				strike = other.strike;
				payoff_coefficients = pnl_vect_copy(other.payoff_coefficients);
				upper_barrier = pnl_vect_copy(other.upper_barrier);
				lower_barrier = pnl_vect_copy(other.lower_barrier);
			}
			return *this;
		}

		~BarrierOptionParameters()
		{
			pnl_vect_free(&payoff_coefficients);
			pnl_vect_free(&upper_barrier);
			pnl_vect_free(&lower_barrier);	
		}
	};

}