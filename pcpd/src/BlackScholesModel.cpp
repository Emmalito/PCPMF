#include <iostream>
#include <cmath>
#include "BlackScholesModel.hpp"

using namespace std;
namespace Couverture {

    BlackScholesModel::BlackScholesModel(int size, double r, double rho, PnlVect* trend_, PnlVect* sigma, PnlVect* spot )
    {
        size_ = size;
        r_ = r;
        sigma_ = sigma;
        spot_ = spot;
        trend_ = trend_;

        //Creation de Gamma et de sa factorisation de Cholesky
        chol_corr_ =  pnl_mat_create_from_scalar(size_,size_,rho);
        pnl_mat_set_diag(chol_corr_, 1, 0);
        pnl_mat_chol(chol_corr_);
    }

    void BlackScholesModel::asset(PnlMat* path, double T, int nbTimeSteps, PnlRng* rng)
    {
        PnlMat* past = pnl_mat_create(1, size_);
        pnl_mat_set_row(past, spot_, 0);
        asset(path, 0.0, T, nbTimeSteps, rng, past);
        pnl_mat_free(&past);
    }

    void BlackScholesModel::asset(PnlMat* path, double t, double T, int nbTimeSteps, PnlRng* rng, const PnlMat* past) 
    {
        pnl_mat_set_subblock(path, past, 0, 0);

        int k = (int) floor(t * nbTimeSteps / T);

        double timeStep = ( (k+1)*T/nbTimeSteps ) - t;

        double factorExprStep = exp(r_*(timeStep));
        double sqrtStep = sqrt(timeStep);

        PnlVect* lastSpots = pnl_vect_create(size_);
        pnl_mat_get_row(lastSpots, past, past->m-1);        

        PnlVect* corr_d = pnl_vect_create(size_);
        PnlVect* normalVect_2 = pnl_vect_create(size_);

        /* Itérations restantes */
        for (int i=k+1; i < path->m; i++)
        {
            pnl_vect_rng_normal(normalVect_2, size_, rng);
            simulate_directions(lastSpots, normalVect_2, corr_d, timeStep, factorExprStep, sqrtStep);
            pnl_mat_set_row(path, lastSpots, i);

            timeStep = T / nbTimeSteps;
            factorExprStep = exp(r_*(timeStep));
            sqrtStep = sqrt(timeStep);
        }
        pnl_vect_free(&lastSpots);
		pnl_vect_free(&normalVect_2);
        pnl_vect_free(&corr_d);

    }

    void BlackScholesModel::shiftAsset(PnlMat* shift_path, const PnlMat* path, int d, double h, double t, double timeStep) {
        pnl_mat_set_subblock(shift_path, path, 0, 0);
        PnlVect* shift = pnl_vect_create_from_scalar(shift_path->m, 1.0);
		int k = (int) floor( t / timeStep);
		// if (fmod(t, timeStep) != 0) {
		// 	k++;
		// }
		//shift_path->m - ceil(t / timeStep)
        PnlVect* sub_shift = pnl_vect_create_from_scalar( (shift_path->m )- k , 1.0+h);

		
		//ceil(t / timeStep)
        pnl_vect_set_subblock(shift, sub_shift, k );
        PnlVect* shifted_asset = pnl_vect_create(shift_path->m);
        pnl_mat_get_col(shifted_asset, shift_path, d);
        pnl_vect_mult_vect_term(shifted_asset, shift);
        pnl_mat_set_col(shift_path, shifted_asset, d);

		// if (t == 2) {
		// 	pnl_mat_print(path);
		// 	cout<<"----"<< k <<"---"<<endl;
		// 	pnl_mat_print(shift_path);
		// 	exit(0);
		// }
        
        pnl_vect_free(&shifted_asset);
        pnl_vect_free(&shift);
        pnl_vect_free(&sub_shift);
    }

    PnlMat* BlackScholesModel::simul_market(int H, double T,  PnlRng* rng)
    {
        double timeStep = T/H;
        PnlMat* marketSimulation = pnl_mat_create_from_zero(H + 1, size_);
        pnl_mat_set_row(marketSimulation, spot_, 0);

        //Simulation vecteur Gaussien centré réduit
        PnlVect* normalVect = pnl_vect_create(size_);

        PnlVect* corr_d = pnl_vect_create(size_);
        double sqrtStep = sqrt(timeStep);

        PnlVect* spots = pnl_vect_copy(spot_);
        for (int i = 1; i < H + 1; i++)
        {
            pnl_vect_rng_normal(normalVect, size_, rng);
            simulate_directions_market(spots, normalVect, corr_d, timeStep, sqrtStep);
            pnl_mat_set_row(marketSimulation, spots, i);
        }
        pnl_vect_free(&spots);
        pnl_vect_free(&normalVect);
        pnl_vect_free(&corr_d);
        return marketSimulation;
    }

    void BlackScholesModel::simulate_directions( PnlVect* spots, PnlVect* normalVect, PnlVect* corr_d , double timeStep, double factorExprStep, double sqrtStep)
    {
        for (int d = 0; d < size_; d++)
        {
            //Calcul des directions
            pnl_mat_get_row(corr_d, chol_corr_, d);
            double sigma_d = pnl_vect_get(sigma_, d);
            double factor =  factorExprStep* exp(sigma_d*(-sigma_d*timeStep/2 + sqrtStep*pnl_vect_scalar_prod(corr_d, normalVect)));
            double stid = factor * pnl_vect_get(spots, d);
			pnl_vect_set(spots, d, stid);
        }
    }

    void BlackScholesModel::simulate_directions_market(PnlVect* spots, PnlVect* normalVect, PnlVect* corr_d , double timeStep, double sqrtStep)
    {
        for (int d = 0; d < size_; d++)
        {
            //Calcul des directions
            pnl_mat_get_row(corr_d, chol_corr_, d);
            double sigma_d = pnl_vect_get(sigma_, d);
            double trend_d = pnl_vect_get(trend_, d);
            double factor =  exp((trend_d - sigma_d*sigma_d/2)*timeStep + sqrtStep*pnl_vect_scalar_prod(corr_d, normalVect));
            pnl_vect_set(spots, d, factor * pnl_vect_get(spots, d));
        }
    }
}