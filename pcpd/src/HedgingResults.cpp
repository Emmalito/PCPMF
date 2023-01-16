#include "HedgingResults.hpp"
#include <iostream>

std::ostream&
operator<<(std::ostream& stm, const HedgingResults& res)
{
    stm << '{' << "\"initialPrice\": " << res.initialPrice << ", \"initialPriceStdDev\": " << res.initialPriceStdDev << ", \"finalPnL\": " << res.finalPnL << '}';
    return stm;
}