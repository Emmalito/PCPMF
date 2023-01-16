#pragma once

#include "RandomGeneration.hpp"
#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"

namespace generators
{

	class PnlRandomGeneration : public RandomGeneration
	{
	private:
		PnlRng *pnlRandomGen;

	public:
		PnlRandomGeneration();
		void get_one_gaussian_sample(PnlVect * const into) const;
		~PnlRandomGeneration() { pnl_rng_free(&pnlRandomGen); }
	};

}