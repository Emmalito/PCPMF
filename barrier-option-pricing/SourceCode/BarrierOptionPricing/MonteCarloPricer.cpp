#include <cmath>
#include "MonteCarloPricer.hpp"
#include "MonteCarloRoutine.hpp"
#include "pnl/pnl_matrix.h"

using namespace options;
using namespace models;



void MonteCarloPricer::price(const UnderlyingModel & underlying_model, const BarrierOption &option, PnlVect * spots, double &price, double &confidence_interval) const
{
	MonteCarloRoutineAtOrigin mco(underlying_model, option, sample_number, spots);
	mco.price(price, confidence_interval);
}

void MonteCarloPricer::price_at(const double time, const models::UnderlyingModel & underlying_model, const options::BarrierOption &option, PnlMat * past, double &price, double &confidence_interval) const
{
	MonteCarloRoutineAtTimeT mct(underlying_model, option, sample_number, past, time);
	mct.price(price, confidence_interval);
}

