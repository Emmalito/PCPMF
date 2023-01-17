#include <iostream>
#include <ctime>
#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"

#include "PricingResults.hpp"
#include "BlackScholesModel.hpp"
#include "Option.hpp"
#include "BarrierMFOption.hpp"
#include "MonteCarlo.hpp"

using namespace std;

int main(int argc, char** argv)
{
    /************* Déclaration des paramètres de fonctions *************/
    string type;
    size_t n_samples = 10000;
    int size = 4, nbTimeSteps = 150;
    double r = 0.01, rho = 0.08, T = 0.4246031746031746;
    PnlVect* sigma = pnl_vect_create_from_list(4);
    pnl_vect_set(sigma, 0, 0.3);
    pnl_vect_set(sigma, 1, 0.3);
    pnl_vect_set(sigma, 2, 0.3);
    pnl_vect_set(sigma, 3, 0.3);
    PnlVect* strikes = pnl_vect_create(4);
    pnl_vect_set(strikes, 0, 10.0);
    pnl_vect_set(strikes, 1, 10.1);
    pnl_vect_set(strikes, 2, 10.2);
    pnl_vect_set(strikes, 3, 10.3);
    PnlVect* dates = pnl_vect_create(4);
    pnl_vect_set(dates, 0, 0.1626984126984127);
    pnl_vect_set(dates, 1, 0.25396825396825395);
    pnl_vect_set(dates, 2, 0.3333333333333333);
    pnl_vect_set(dates, 3, 0.4246031746031746);
    PnlVect* spots = pnl_vect_create(4);
    pnl_vect_set(spots, 0, 8.0);
    pnl_vect_set(spots, 1, 8.0);
    pnl_vect_set(spots, 2, 8.0);
    pnl_vect_set(spots, 3, 8.0);

    PnlRng* rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));

    /************* Construction de Black Scholes et de l'option *************/
    pnl_mat_create(nbTimeSteps+1, size);
    BlackScholesModel* blackScholesModel = new BlackScholesModel(size, r, rho, sigma, spots);
    Option* option = new BarrierMFOption(T, nbTimeSteps, size, r, strikes, dates);

    /************* Construction de MonteCarlo *************/
    MonteCarlo* monteCarlo = new MonteCarlo(blackScholesModel, option, rng, 0.1, n_samples);

    double prix, prix_std_dev;
    /************* Calcul du prix, du delta et Display du resultat *************/
    PnlVect* delta = pnl_vect_create(option->size_);
    PnlVect* delta_std_dev = pnl_vect_create(option->size_);
    monteCarlo->price_and_delta(prix, prix_std_dev, delta, delta_std_dev);

    PricingResults res(prix, prix_std_dev, delta, delta_std_dev);
    std::cout << res << std::endl;

    /************* Libération mémoire *************/
    pnl_vect_free(&spots);
    pnl_vect_free(&sigma);
    pnl_vect_free(&strikes);
    pnl_vect_free(&dates);
    pnl_vect_free(&delta);
    pnl_vect_free(&delta_std_dev);
    pnl_rng_free(&rng);
    delete(option);
    delete(blackScholesModel);
    delete(monteCarlo);
    return 0;
}
