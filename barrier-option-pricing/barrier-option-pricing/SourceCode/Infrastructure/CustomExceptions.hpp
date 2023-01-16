#pragma once

#include <exception>

namespace infrastructure
{

	class ModelMismatchException : public std::exception
	{

	};

	class ConstraintException : public std::exception
	{

	};

}