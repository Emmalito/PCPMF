#pragma once
#include <string>
#include "pnl/pnl_vector.h"

class PricingResults
{
  private:
    double price;
    const PnlVect* delta;
    double priceStdDev;
    const PnlVect* deltaStdDev;

  public:
    PricingResults(double price, double priceStdDev, const PnlVect* const delta, const PnlVect* const deltaStdDev)
      : price(price)
      , priceStdDev(priceStdDev)
      , delta(delta)
      , deltaStdDev(deltaStdDev)
    {
    }

    ~PricingResults() {}

    PricingResults(const PricingResults& pr)
      : price(pr.price)
      , priceStdDev(pr.priceStdDev)
      , delta(pr.delta)
      , deltaStdDev(pr.deltaStdDev)
    {
    }

    double Price() const
    {
        return price;
    }

    double PriceStdDev() const
    {
        return priceStdDev;
    }

    const PnlVect* const Delta() const
    {
        return delta;
    }

    const PnlVect* const DeltaStdDev() const
    {
        return deltaStdDev;
    }

    friend std::ostream& operator<<(std::ostream& stm, const PricingResults& res);
};
