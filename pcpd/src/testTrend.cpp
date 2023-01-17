#include <iostream>
#include <ctime>
#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"

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
    PnlVect *spot, *sigma, *divid, *weights, *delta, *delta_std_dev, *trend= nullptr;
    string type;
    int size, nbTimeSteps, hedgingTimeSteps;
    size_t n_samples;

    /************ Construction du parseur et extraction des paramètres ****************/
    char* infile = argv[1];
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
    P->extract("trend", trend, size);
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


    /************ Construction de Black Scholes et de l'option **************/
    BlackScholesModel* blackScholesModel = new BlackScholesModel(size, r, rho, sigma, spot, trend);
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
    //MonteCarlo* monteCarlo = new MonteCarlo(blackScholesModel, option, rng, fdstep, n_samples);

    /************** Calcul du prix, du P&L et Display du resultat ******************/
    /*monteCarlo->price(initialPrice, initialPriceStdDev);
    finalPnL = monteCarlo->calculate_p_and_l(path, hedgingTimeSteps);
    HedgingResults res (initialPrice, initialPriceStdDev, finalPnL);
    std::cout << res << std::endl;
    */

    PnlMat* path = pnl_mat_create(hedgingTimeSteps + 1, blackScholesModel->size_);
    blackScholesModel->simul_market(path, hedgingTimeSteps, T, rng);

    for(int i = 0; i < hedgingTimeSteps; i++)
    {
        std::cout << pnl_mat_get(path, i, 0) << std::endl;
    }
    /************** Libération mémoire ********************/
    pnl_vect_free(&spot);
    pnl_vect_free(&sigma);
    pnl_vect_free(&weights);
    pnl_vect_free(&divid);
    pnl_rng_free(&rng);
    delete(option);
    delete(blackScholesModel);
    // delete(monteCarlo);
    delete(P);
    return 0;
}
