#include "BarrierMFOption.hpp"
#include <cmath>
#include <iostream>

using namespace std;

BarrierMFOption::BarrierMFOption(double T_, int nbTimeSteps_, int size_, double r_, PnlVect *strikes_, PnlVect *dates_)
{
    T = T_;
    nbTimeSteps = nbTimeSteps_;
    size = size_;
    r = r_;
    strikes = pnl_vect_copy(strikes_);
    dates = pnl_vect_copy(dates_);
}

double BarrierMFOption::payoff(const PnlMat* path) {
    double res = 0.0, exponential;
    int C_i_1 = 1, i = 0;
    while (C_i_1 == 1 && i < path->m)
    {
        exponential = exp(r*(T-pnl_vect_get(dates, i)));
        res = exponential * max(pnl_mat_get(path, i, i) - pnl_vect_get(strikes, i), 0.0);
        if (res > 0) C_i_1 = 0;
        i++;
    }
    return res;
}

BarrierMFOption::~BarrierMFOption() {
    pnl_vect_free(&strikes);
    pnl_vect_free(&dates);
}
