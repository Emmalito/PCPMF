#include "BasketOption.hpp"

namespace Couverture {
    BasketOption::BasketOption(double T, int nbTimeSteps, int size, PnlVect *lambdas, PnlVect *spots, double strike) {
        T_ = T;
        nbTimeSteps_ = nbTimeSteps;
        size_ = size;
        lambdas_ = lambdas;
        spots_ = spots;
        strike_ = strike;
    }

    double BasketOption::payoff(const PnlMat* path) {
        double somme = 0;
        double STd;
        double lambda_d;
        PnlVect *lastRow = pnl_vect_create(this->size_);
        pnl_mat_get_row(lastRow, path, path->m-1);
        somme=pnl_vect_scalar_prod(lastRow, lambdas_);
		pnl_vect_free(&lastRow);
        return max(somme-this->strike_, 0.0);
    }
}