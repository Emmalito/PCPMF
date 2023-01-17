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

// Attention ! Ne marche que pour les options asian & perf !

// Renvoie ti tel que ti = max{tk < t ; k}
double plusProche_ti(double t, double T, int nbTimeSteps)
{
    double ti = 0;

    for(ti = 0; ti <= t; ti+=T/nbTimeSteps){continue;}

    return ti - T/nbTimeSteps;
    
}

// renvoie le i d'un ti
int geti(double ti, double T, int nbTimeSteps)
{
    return (int) (ti * nbTimeSteps/T);
}

bool is_t_obs(double t, double T, int nbTimeSteps)
{
    return plusProche_ti(t, T, nbTimeSteps) == t;
}

int main(int argc, char** argv)
{
    // Déclaration des paramètres de fonctions
    double T, r, strike, rho, fdstep, prix, prix_std_dev;
    PnlVect *spot, *sigma, *divid, *weights, *delta, *delta_std_dev;
    string type;
    int size, nbTimeSteps;
    size_t n_samples;
    // Construction du parseur et extraction des paramètres
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
    P->extract("payoff coefficients", weights, size);
    if (P->extract("dividend rate", divid, size, true) == false) {
        divid = pnl_vect_create_from_zero(size);
    }
    if (type != "performance"){
        P->extract("strike", strike);
    }
    P->extract("sample number", n_samples);

    // Construction de Black Scholes
    BlackScholesModel* blackScholesModel = new BlackScholesModel(size, r, rho, sigma, spot);

    // Construction de l'option
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

    // Construction de MonteCarlo
    PnlRng* rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));
    MonteCarlo* monteCarlo = new MonteCarlo(blackScholesModel, option, rng, fdstep, n_samples);

    // Calcul du prix

    // Calcul du past
    PnlMat* past = pnl_mat_create(option->nbTimeSteps_ + 1 ,blackScholesModel->size_);
    // je génère une trajectoire de sous jacents qui admettons a été observée sur le marché.
    // Le but va être d'étudier le prix d'une option sur les sous sous jacents. L'interret est que 
    // puisque cette trajectoire est supposée extraite du marché, le payoff de l'option sur ces sous 
    // jacents est connu. La manière de procéder va donc être de fournir une sous-matrice de ces trajectoires
    // de plus en plus grande, et d'observer que le prix estimé converge bien vers le payoff de l'option.
    // puisque la classe Monte Carlo ne possède pas de Sharingan, il ne connait pas le payoff. Ainsi s'il s'en
    // rapproche c'est que l'estimateur fait bien son taff.
     
    blackScholesModel->asset(past, option->T_, nbTimeSteps, rng);
    int nbEvals = 10;
    double evalStep = T/nbEvals;

    // Calcul du prix en t=0:

    std::cout << "----------- Date t = 0" << std::endl;
    monteCarlo->price(prix, prix_std_dev);

    PricingResults res(prix, prix_std_dev, delta, delta_std_dev);
    std::cout << "Prix Only "<< prix  << std::endl;
    std::cout << "Payoff "<< option->payoff(past)  << std::endl;
    std::cout << "Ecart type "<< prix_std_dev  << std::endl;

    // Calcul du prix en 0 < t < T:

    for(double t = evalStep; t <= T; t += evalStep)
    {
        std::cout << "----------- Date t = "<< t << std::endl;

        if(is_t_obs(t, option->T_, nbTimeSteps))
        {
            PnlMat* cutted_past = pnl_mat_create((int) (t * nbTimeSteps/T) + 1 ,blackScholesModel->size_);
            pnl_mat_extract_subblock(cutted_past, past, 0, cutted_past->m, 0, cutted_past->n);
            monteCarlo->price(cutted_past, t, prix, prix_std_dev);

            PricingResults res(prix, prix_std_dev, delta, delta_std_dev);
            std::cout << "Prix Only "<< prix  << std::endl;
            std::cout << "Payoff "<< option->payoff(past)  << std::endl;
            std::cout << "Ecart type "<< prix_std_dev  << std::endl;
            pnl_mat_free(&cutted_past);
        }
        else
        {
            PnlMat* cutted_past = pnl_mat_create(geti(plusProche_ti(t, option->T_, nbTimeSteps), option->T_, nbTimeSteps) + 2 ,blackScholesModel->size_);
            //pnl_mat_extract_subblock(cutted_past, past, 0, cutted_past->m - 1, 0, cutted_past->n);
            PnlMat* sb = pnl_mat_create(cutted_past->m - 1 ,blackScholesModel->size_);
            pnl_mat_extract_subblock(sb, past, 0, cutted_past->m - 1, 0, cutted_past->n);
            pnl_mat_set_subblock(cutted_past, sb, 0, 0);


            blackScholesModel->asset_at_time_i(cutted_past->m - 1, cutted_past, t - plusProche_ti(t, option->T_, nbTimeSteps), rng);
            monteCarlo->price(cutted_past, t, prix, prix_std_dev);

            PricingResults res(prix, prix_std_dev, delta, delta_std_dev);
            std::cout << "Prix Only "<< prix  << std::endl;
            std::cout << "Payoff "<< option->payoff(past)  << std::endl;
            std::cout << "Ecart type "<< prix_std_dev  << std::endl;
            pnl_mat_free(&cutted_past);
        }
    }

    // Libération mémoire
    pnl_vect_free(&spot);
    pnl_vect_free(&sigma);
    pnl_vect_free(&weights);
    pnl_vect_free(&divid);
    pnl_rng_free(&rng);
    pnl_mat_free(&past);
    delete(option);
    delete(blackScholesModel);
    delete(monteCarlo);
    delete(P);
    return 0;
}
