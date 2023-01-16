#include "testPriceT.hpp"
#include <cmath>

using namespace Couverture;

int main(int argc, char** argv)
{ 
	if (argc != 2) {
		 cout << "one file needed" << endl;
		 return 0;
	}

	string key;

	double T, r, strike;
    PnlVect *spot, *sigma, *lambdas;
	
    string type;
    int size, nbTimeSteps;
    size_t n_samples;
	char* infile = argv[1];
	double rho;	
	size_t M ;
	PnlRng* rng = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	// read params
	Param* P = new Parser(infile);
	P->extract("option type", type);
    P->extract("maturity", T);
    P->extract("option size", size);
    P->extract("spot", spot, size);
    P->extract("volatility", sigma, size); 
    P->extract("interest rate", r);
	P->extract("option type", key);
	P->extract("correlation", rho);
	P->extract("sample number", M);	
	P->extract("timestep number", nbTimeSteps);
	P->extract("payoff coefficients", lambdas, size);

	PnlVect *deltas = pnl_vect_create(size);
	double pricet=0;
    double price0T_t = 0;
	double stdDev=0;

    double h = 0.1;

    double t=T;

	string asian = "asian";
	string basket = "basket";
	string performance = "performance";
	PnlVect* trend = pnl_vect_create_from_scalar(size, 0.0);
	BlackScholesModel blackScholesModel(size, r, rho, trend, sigma, spot);

    int ligne = ceil(t*nbTimeSteps/T);
    PnlMat* past = pnl_mat_create(ligne+1, size);
	
    blackScholesModel.asset(past, t, ligne, rng	);

	PnlVect* spotLigne = pnl_vect_create(size);
	pnl_mat_get_row(spotLigne, past, ligne);

	if (asian.compare(key) == 0) {
		P->extract("strike", strike);
		AsianOption asianOption(T, nbTimeSteps, size, lambdas, spot, strike);
		MonteCarlo monteCarlo(&blackScholesModel, &asianOption, rng, h,  M );
		cout << "Payoff = "<<asianOption.payoff(past)<<endl;
        monteCarlo.price(past, t, pricet, stdDev);
	} else if (basket.compare(key) == 0) {
		P->extract("strike", strike);
		BlackScholesModel blackScholesModel(size, r, rho, trend, sigma, spot);
		BasketOption basketOption(T, nbTimeSteps, size, lambdas, spot, strike);
		MonteCarlo monteCarlo(&blackScholesModel, &basketOption, rng, h,  M );
		cout << "Payoff = "<<basketOption.payoff(past)<<endl;
        monteCarlo.price(past, t, pricet, stdDev);
	} else if (performance.compare(key) == 0) {
		BlackScholesModel blackScholesModel(size, r, rho, trend, sigma, spot);
		PerfOption perfOption(T, nbTimeSteps, size, lambdas, spot, strike);
		MonteCarlo monteCarlo(&blackScholesModel, &perfOption, rng, h,  M );
		cout << "Payoff = "<<perfOption.payoff(past)<<endl;
        monteCarlo.price(past, t, pricet, stdDev);
	} else {	
		cout<<"La clé n'est pas valide."<<endl;
		return 1;
	}	
	
	PricingResults res(pricet, stdDev, deltas, deltas);
	std::cout << res << std::endl;

	pnl_vect_free(&spot);
    pnl_vect_free(&sigma);
	pnl_vect_free(&lambdas);
	pnl_rng_free(&rng);
	delete P;
	return 0;
}

