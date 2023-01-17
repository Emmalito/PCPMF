#include "BarrierMFOption.hpp"
#include <cmath>
#include <iostream>

using namespace std;

BarrierMFOption::BarrierMFOption(double T, int nbTimeSteps, int size, double r, PnlVect *strikes, PnlVect *dates)
{
    T_ = T;
    nbTimeSteps_ = nbTimeSteps;
    size_ = size;
    r_ = r;
    strikes_ = strikes;
    dates_ = dates;
}

double BarrierMFOption::payoff(const PnlMat* path) {
    double res = 0.0, Sti, Ki, exponential;
    int C_i_1 = 1, i = 0, ti = 0;
    while (C_i_1 == 1 && i < size_)
    {
        ti = (int) (pnl_vect_get(dates_, i) * (double) path->m / T_ );
        if (ti > path->m-1) ti = path->m-1;
        exponential = exp(r_*(T_-pnl_vect_get(dates_, i)));
        res = exponential * max(pnl_mat_get(path, ti, i) - pnl_vect_get(strikes_, i), 0.0);
        if (res > 0) C_i_1 = 0;
        i++;
    }
    return res;
}

BarrierMFOption::~BarrierMFOption() {
    pnl_vect_free(&strikes_);
    pnl_vect_free(&dates_);
}
