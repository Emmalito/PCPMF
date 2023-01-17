#include <iostream>
#include <ctime>
#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_finance.h"

#include "jlparser/parser.hpp"

#include "HedgingResults.hpp"
#include "BlackScholesModel.hpp"
#include "Option.hpp"
#include "AsianOption.hpp"
#include "BasketOption.hpp"
#include "PerformanceOption.hpp"
#include "MonteCarlo.hpp"

using namespace std;

int main(int argc, char** argv)
{

    /************* Déclaration des paramètres de fonctions **************/
    double T, r, strike = 0, rho, fdstep, initialPrice, initialPriceStdDev, finalPnL;
    PnlVect *spot, *sigma, *divid, *weights, *delta, *delta_std_dev;
    string type;
    int size, nbTimeSteps, hedgingTimeSteps;
    size_t n_samples;

    /************ Construction du parseur et extraction des paramètres ****************/
    char* infile = argv[2];
    Param* P = new Parser(infile);
    P->extract("option type", type);
    P->extract("maturity", T);
    P->extract("option size", size);
    P->extract("spot", spot, size);
    P->extract("volatility", sigma, size);
    P->extract("interest rate", r);
    P->extract("correlation", rho);
    P->extract("timestep number", nbTimeSteps);
    P->extract("hedging dates number", hedgingTimeSteps);
    P->extract("payoff coefficients", weights, size);
    if(P->extract("fd step", fdstep, true) == false){
        fdstep = 0.1;
    }
    if (P->extract("dividend rate", divid, size, true) == false) {
        divid = pnl_vect_create_from_zero(size);
    }
    if (type != "performance"){
        P->extract("strike", strike);
    }
    P->extract("sample number", n_samples);

    /********* Construction de la matrice path, simulation du marché ***************/
    PnlMat* path = pnl_mat_create_from_file(argv[1]);

    /************ Construction de Black Scholes et de l'option **************/
    BlackScholesModel* blackScholesModel = new BlackScholesModel(size, r, rho, sigma, spot);
    Option* option;
    if (type == "asian"){
        option = new AsianOption(T, nbTimeSteps, size, strike, weights);
    } else if (type == "basket"){
        option = new BasketOption(T, nbTimeSteps, size, strike, weights);
    } else if (type == "performance"){
        option = new PerformanceOption(T, nbTimeSteps, size, weights);
    } else {
        throw std::invalid_argument("Option type not handled by Hedger");
    }

    /*************** Construction de MonteCarlo *************/
    PnlRng* rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));
    MonteCarlo* monteCarlo = new MonteCarlo(blackScholesModel, option, rng, fdstep, n_samples);

    delta = pnl_vect_create(option->size_);
    delta_std_dev = pnl_vect_create(option->size_);

    /************** Calcul du prix, du P&L et Display du resultat ******************/
    monteCarlo->price(initialPrice, initialPriceStdDev);
    monteCarlo->delta(delta, delta_std_dev);
    double call_price;
    double call_delta;
    pnl_cf_call_bs(pnl_vect_get(spot, 0), strike, T, r, pnl_vect_get(divid, 0), pnl_vect_get(sigma, 0), &call_price, &call_delta);
    std::cout << "Price = " << initialPrice << " VS " << call_price << std::endl;
    std::cout << "Delta = " << pnl_vect_get(delta, 0) << " VS " << call_delta << std::endl;
    
    int i = 1;
    for(double tau = T/hedgingTimeSteps; tau < T; tau += T/hedgingTimeSteps)
    {
        PnlMat* past = monteCarlo->extract_past(path, hedgingTimeSteps, tau);
        monteCarlo->price(past, tau, initialPrice, initialPriceStdDev);
        monteCarlo->delta(past, tau, delta, delta_std_dev);

        double spot_price = pnl_mat_get(path, i, 0);
        pnl_cf_call_bs(spot_price, strike, T - tau, r, 0, pnl_vect_get(sigma, 0), &call_price, &call_delta);
        std::cout << "Spot at day " << i << " " << pnl_mat_get(path, i, 0) << std::endl;
        std::cout << "Price = " << initialPrice << " VS " << call_price << std::endl;
        std::cout << "Delta = " << pnl_vect_get(delta, 0) << " VS " << call_delta << std::endl;
        i++;
    }

    /************** Libération mémoire ********************/
    pnl_vect_free(&spot);
    pnl_vect_free(&sigma);
    pnl_vect_free(&weights);
    pnl_vect_free(&divid);
    pnl_rng_free(&rng);
    delete(option);
    delete(blackScholesModel);
    delete(monteCarlo);
    delete(P);
    return 0;
}
