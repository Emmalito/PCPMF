//
// Created by lionel on 12/09/22.
//

#include "AsianOption.hpp"

AsianOption::AsianOption(double T, int nbTimeSteps, int size, double k, PnlVect * weights)
{
    T_ = T;
    nbTimeSteps_ = nbTimeSteps;
    size_ = size;
    k_ = k;
    weights_ = pnl_vect_copy(weights);
}

AsianOption::~AsianOption(){
    pnl_vect_free(&weights_);
}

/*
 * \frac{1}{N+1} \sum_{i}\sum_{d}\lambda_d \times S_{t_i, d}
 */
double AsianOption::payoff(const PnlMat* path)
{
    double result = 0;
    for (int i = 0; i < path->m; i++){
        PnlVect ligne_i = pnl_vect_wrap_mat_row(path, i);
        result += pnl_vect_scalar_prod(weights_, &ligne_i);
        /*for (int d = 0; d < weights_->size; d++) {
            result += pnl_vect_get(weights_, d) * pnl_mat_get(path, i, d);
        }*/
    }
    result = result / path->m - k_;
    return result > 0 ? result : 0;

}