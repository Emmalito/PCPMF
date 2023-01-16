#include "PnlRandomGeneration.hpp"
#include <time.h>

using namespace generators;

PnlRandomGeneration::PnlRandomGeneration()
{
	pnlRandomGen = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_sseed(pnlRandomGen, time(NULL));
}

void PnlRandomGeneration::get_one_gaussian_sample(PnlVect * const into) const
{
	pnl_vect_rng_normal(into, into->size, pnlRandomGen);
}