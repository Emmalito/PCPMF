#include <iostream>
#include "PricingResults.hpp"
#include "PnlVectToJson.hpp"

std::ostream&
operator<<(std::ostream& stm, const PricingResults& res)
{
    stm << '{' << "\"price\": " << res.price << ", \"priceStdDev\": " << res.priceStdDev << ", \"delta\": ";
    stm << res.delta << ", \"deltaStdDev\": " << res.deltaStdDev << '}';
    return stm;
}