#pragma once
#include "RandomGeneration.hpp"



class ConstantRandomGeneration : public generators::RandomGeneration
{
public:
	ConstantRandomGeneration()
	{}

	~ConstantRandomGeneration()
	{}

	void get_one_gaussian_sample(PnlVect * const into) const
	{
		int size = into->size;
		for (int i = 0; i < size; i++)
		{
			LET(into, i) = 0;
		}
	}
};