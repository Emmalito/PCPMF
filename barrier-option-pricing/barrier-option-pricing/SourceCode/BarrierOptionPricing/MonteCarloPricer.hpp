#pragma once
#include "pnl/pnl_vector.h"
#include "RandomGeneration.hpp"
#include "BarrierOption.hpp"
#include "UnderlyingModel.hpp"

class MonteCarloPricer
{
private:
	const unsigned long sample_number;

public:
	MonteCarloPricer(const unsigned long sample_nb) : sample_number(sample_nb) {};
	void price(const models::UnderlyingModel & underlying_model, const options::BarrierOption &option, PnlVect * spots, double &price, double &confidence_interval) const;
	void price_at(const double time, const models::UnderlyingModel & underlying_model, const options::BarrierOption &option, PnlMat * past, double &price, double &confidence_interval) const;
};