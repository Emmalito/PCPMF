#include "BarrierMFOption.hpp"
#include <cmath>
#include <iostream>

using namespace std;

BarrierMFOption::BarrierMFOption(double T, int nbTimeSteps, int size, double r, PnlVect *strikes, PnlVect *dates)
{
    T = T;
    nbTimeSteps = nbTimeSteps;
    size = size;
    r = r;
    strikes = strikes;
    dates = dates;
}

double BarrierMFOption::payoff(const PnlMat* path) {
    double res = 0.0, Sti, Ki, exponential;
    int C_i_1 = 1, i = 0, ti = 0;
    while (C_i_1 == 1 && i < size)
    {
        ti = (int) (pnl_vect_get(dates, i) * (double) path->m/T);
        if (ti > path->m-1) ti = path->m-1;
        exponential = exp(r*(T-pnl_vect_get(dates, i)));
        res = exponential * max(pnl_mat_get(path, ti, i) - pnl_vect_get(strikes, i), 0.0);
        if (res > 0) C_i_1 = 0;
        i++;
    }
    return res;
}

BarrierMFOption::~BarrierMFOption() {
    //pnl_vect_free(&strikes);
    //pnl_vect_free(&dates);
}
