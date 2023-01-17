//
// Created by lionel on 12/09/22.
//

#include "PerformanceOption.hpp"

PerformanceOption::PerformanceOption(double T, int nbTimeSteps, int size, PnlVect * weights)
{
    T_ = T;
    nbTimeSteps_ = nbTimeSteps;
    size_ = size;
    weights_ = pnl_vect_copy(weights);
}
PerformanceOption::~PerformanceOption(){
    pnl_vect_free(&weights_);
}

double PerformanceOption::payoff(const PnlMat* path)
{
    double result = 0;
    // Parcours du temps : boucle en i du poly
    for (int i = 1; i < path->m; i++){
        double num = 0, deno = 0, frac;
        // Parcours des weights ou des actifs : boucle en d du poly
        for (int d = 0; d < weights_->size; d++){
            num += pnl_vect_get(weights_, d) * pnl_mat_get(path, i, d);
            deno += pnl_vect_get(weights_, d) * pnl_mat_get(path, i - 1 , d);
        }
        frac = num / deno - 1;
        result += frac > 0 ? frac : 0;
    }
    return result + 1;
}