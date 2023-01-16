#pragma once
#include "BarrierOptionInputParser.hpp"

class FakeBarrierOptionInputParser : public input_parsers::BarrierOptionInputParser
{
private:
	PnlVect * up_barrier, *low_barrier, *pay_coeff;
public:
	FakeBarrierOptionInputParser()
	{
		int nbu = 3;
		pay_coeff = pnl_vect_create(nbu);
		LET(pay_coeff, 0) = 1;
		LET(pay_coeff, 1) = 2;
		LET(pay_coeff, 2) = 3;
		up_barrier = pnl_vect_create(nbu);
		LET(up_barrier, 0) = 3;
		LET(up_barrier, 1) = 3.25;
		LET(up_barrier, 2) = 3.5;
		low_barrier = pnl_vect_create(nbu);
		LET(low_barrier, 0) = 1;
		LET(low_barrier, 1) = 1.25;
		LET(low_barrier, 2) = 1.5;		
	};
	
	~FakeBarrierOptionInputParser()
	{
		pnl_vect_free(&low_barrier);
		pnl_vect_free(&up_barrier);
		pnl_vect_free(&pay_coeff);
		low_barrier = up_barrier = pay_coeff = nullptr;
	}

	int get_underlying_number() const { return 3; }
	double get_maturity() const { return 2; }
	double get_strike() const { return 2; }
	PnlVect * get_payoff_coefficients() const { return pay_coeff; }
	PnlVect * get_lower_barrier() const { return low_barrier; }
	PnlVect * get_upper_barrier() const { return up_barrier; }
	
};