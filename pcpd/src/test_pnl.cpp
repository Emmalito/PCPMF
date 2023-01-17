#include <iostream>
#include <ctime>
#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"

#include "jlparser/parser.hpp"

#include "PricingResults.hpp"
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
    double T, r, strike, rho, fdstep, prix, prix_std_dev;
    PnlVect *spot, *sigma, *divid, *weights, *delta, *delta_std_dev;
    string type;
    int size, nbTimeSteps, hedingTimeSteps;
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
    P->extract("hedging dates number", hedingTimeSteps);
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
        throw std::invalid_argument("Option type not handled by pricer0");
    }

    /*************** Construction de MonteCarlo *************/
    PnlRng* rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));
    MonteCarlo* monteCarlo = new MonteCarlo(blackScholesModel, option, rng, fdstep, n_samples);

    /************** Calcul de la PNL ******************/

    PnlMat* path = pnl_mat_create(hedingTimeSteps + 1 ,blackScholesModel->size_);

    std::cout << "P&L :"<< monteCarlo->calculate_p_and_l(path, hedingTimeSteps)  << std::endl;


    


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
