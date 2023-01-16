#pragma once
#include "pnl/pnl_vector.h"

namespace generators
{

	class RandomGeneration
	{
	public:
		virtual void get_one_gaussian_sample(PnlVect * const into) const = 0;
		virtual ~RandomGeneration() {};
	};

}