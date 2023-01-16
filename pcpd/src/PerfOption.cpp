#include "PerfOption.hpp"

namespace Couverture {
    PerfOption::PerfOption(double T, int nbTimeSteps, int size, PnlVect *lambdas, PnlVect *spots, double strike) {
        T_ = T;
        nbTimeSteps_ = nbTimeSteps;
        size_ = size;
        lambdas_ = lambdas;
        spots_ = spots;
    }

    double PerfOption::payoff(const PnlMat* path) {
        double sommeI = 0;
        double sommeIMoins1 = 0;
        double somme = 0;
        double lambda_d;
        double N = this->nbTimeSteps_;
		int length = path->m;
		double step = (path->m)/(N+1);
        PnlVect *rowI = pnl_vect_create(size_);
        PnlVect *rowIMoins1 = pnl_vect_create(size_);
        for (int i=step; i< length; i+=step) {
            pnl_mat_get_row(rowI, path, i);
            sommeI = 0;
            sommeIMoins1 = 0;
            pnl_mat_get_row(rowIMoins1, path, i-step);
            sommeI+=pnl_vect_scalar_prod(rowI, lambdas_);
            sommeIMoins1+=pnl_vect_scalar_prod(rowIMoins1, lambdas_);

            somme += max(sommeI/sommeIMoins1 - 1, 0.0);
        }
        
        pnl_vect_free(&rowI);
		pnl_vect_free(&rowIMoins1);

        return 1 + somme;
    }
}