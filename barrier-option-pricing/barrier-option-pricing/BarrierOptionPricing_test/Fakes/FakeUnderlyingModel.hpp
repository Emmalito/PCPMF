
#include "UnderlyingModel.hpp"


class FakeUnderlyingModel : public models::UnderlyingModel
{
public:
	FakeUnderlyingModel() {};
	FakeUnderlyingModel(int un)
	{
		underlying_number_ = un;
	};
	
};