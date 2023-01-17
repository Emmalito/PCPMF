#pragma once

#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "Option.hpp"


class BasketOption: public Option {
  private:
    PnlVect * weights_; /// tableau des poids des sous-jacents
    double k_; /// strike de l'option

  public:
    BasketOption(double maturity,int nbTimeSteps, int size, double strike, PnlVect * poids);
    ~BasketOption();
    double payoff(const PnlMat* path);
};

