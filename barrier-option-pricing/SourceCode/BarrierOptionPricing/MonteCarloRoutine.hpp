#pragma once
#include "pnl/pnl_matrix.h"
#include "pnl/pnl_vector.h"
#include "RandomGeneration.hpp"
#include "UnderlyingModel.hpp"
#include "BarrierOption.hpp"

class MonteCarloRoutine
{	
protected:
	virtual const PnlMat * const get_generated_path() const = 0;
	const models::UnderlyingModel & underlying_model;
	const options::BarrierOption &option;
	const unsigned long sample_number;
	const double t;
	MonteCarloRoutine(const models::UnderlyingModel & underlying_model,	const options::BarrierOption &option, const unsigned long sample_number, const double t) : 
		underlying_model(underlying_model), option(option), sample_number(sample_number), t(t)
	{}
public:
	void price(double &price, double &confidence_interval) const;
};

///////////////////////////

class MonteCarloRoutineAtOrigin : public MonteCarloRoutine
{
private:
	const PnlVect * const spots;
protected:
	const PnlMat * const get_generated_path() const
	{
		return underlying_model.simulate_asset_paths_from_start(spots);
	}
public:
	MonteCarloRoutineAtOrigin(const models::UnderlyingModel &underlying_model, const options::BarrierOption &option, const unsigned long sample_nb, const PnlVect * const spots) :
		MonteCarloRoutine(underlying_model, option, sample_nb, 0), spots(spots)
	{}

};


////////////////////////////

class MonteCarloRoutineAtTimeT : public MonteCarloRoutine
{
private:
	const PnlMat * const past;
protected:
	const PnlMat * const get_generated_path() const
	{
		return underlying_model.simulate_asset_paths_unsafe(t, past);
	}
public:
	MonteCarloRoutineAtTimeT(const models::UnderlyingModel &underlying_model, const options::BarrierOption &option, const unsigned long sample_nb, const PnlMat * const past, const double t) :
		MonteCarloRoutine(underlying_model, option, sample_nb, t), past(past)
	{}
};