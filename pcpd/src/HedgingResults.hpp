#pragma once
#include <iostream>

class HedgingResults
{
  private:
    double initialPrice;
    double initialPriceStdDev;
    double finalPnL;

  public:
    HedgingResults(double initialPrice, double initialPriceStdDev, double finalPnL)
      : initialPrice(initialPrice)
      , initialPriceStdDev(initialPriceStdDev)
      , finalPnL(finalPnL)
    {
    }

    HedgingResults(const HedgingResults& hr)
      : initialPrice(hr.initialPrice)
      , initialPriceStdDev(hr.initialPriceStdDev)
      , finalPnL(hr.finalPnL)
    {
    }

    double InitialPrice() const
    {
        return initialPrice;
    }

    double InitialPriceStdDev() const
    {
        return initialPriceStdDev;
    }

    double FinalPnL() const
    {
        return finalPnL;
    }

    friend std::ostream& operator<<(std::ostream& stm, const HedgingResults& res);
};
