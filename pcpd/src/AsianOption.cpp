#include "AsianOption.hpp"

namespace Couverture {

    AsianOption::AsianOption(double T, int nbTimeSteps, int size, PnlVect *lambdas, PnlVect *spots, double strike)
    {
        T_ = T;
        nbTimeSteps_ = nbTimeSteps;
        size_ = size;
        lambdas_ = lambdas;
        spots_ = spots;
        strike_ = strike;
    }

    double AsianOption::payoff(const PnlMat* path) {
        double somme = 0;
        double Stid=0;
        double lambda_d;
        double N = this->nbTimeSteps_;
		int length = path->m;
		double step = (path->m)/(N+1);
        PnlVect* ligne = pnl_vect_create(size_);
        for (int i=0; i<length;i+=step) {
            pnl_mat_get_row(ligne, path, i);
            somme+=pnl_vect_scalar_prod(ligne, lambdas_);
        }
        somme/=(N+1);
        pnl_vect_free(&ligne);
        return max(somme-this->strike_, 0.0);  
    }

    AsianOption::~AsianOption() { }
}