#include "testPriceO.hpp"

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
	PnlVect* trend = pnl_vect_create_from_scalar(size, 0.0);

	double price0=0;
	double stdDev=0;
	double h = 0.1;

	PnlVect *delta0 = pnl_vect_create(size);
	PnlVect *deltaDev = pnl_vect_create(size); 
	string asian = "asian";
	string basket = "basket";
	string performance = "performance";
	
	BlackScholesModel blackScholesModel(size, r, rho, trend, sigma, spot);
	if (asian.compare(key) == 0) {
		P->extract("strike", strike);
		AsianOption asianOption(T, nbTimeSteps, size, lambdas, spot, strike);
		MonteCarlo monteCarlo(&blackScholesModel, &asianOption, rng, h,  M );
		monteCarlo.priceDelta(NULL, 0.0, delta0, deltaDev, price0, stdDev);
		// monteCarlo.price(price0, stdDev);
		// monteCarlo.delta(delta0, deltaDev);
	} else if (basket.compare(key) == 0) {
		P->extract("strike", strike);
		BasketOption basketOption(T, nbTimeSteps, size, lambdas, spot, strike);
		MonteCarlo monteCarlo(&blackScholesModel, &basketOption, rng, h,  M );
		monteCarlo.priceDelta(NULL, 0.0, delta0, deltaDev, price0, stdDev);
		// monteCarlo.price(price0, stdDev);
		// monteCarlo.delta(delta0, deltaDev);
	} else if (performance.compare(key) == 0) {
		PerfOption perfOption(T, nbTimeSteps, size, lambdas, spot, strike);
		MonteCarlo monteCarlo(&blackScholesModel, &perfOption, rng, h,  M );
		monteCarlo.priceDelta(NULL, 0.0, delta0, deltaDev, price0, stdDev);
		// monteCarlo.price(price0, stdDev);
		// monteCarlo.delta(delta0, deltaDev);
	} else {
		cout<<"La clé n'est pas valide."<<endl;
		return 1;
	}	
	
	// cout << "the price at t = 0 : " << price0 << endl;
	// cout << "stdDev :  " << stdDev << endl;

	PricingResults res(price0, stdDev, delta0, deltaDev);
	std::cout << res << std::endl;

	// cholesky
	pnl_mat_free(&(blackScholesModel.chol_corr_));

	pnl_vect_free(&delta0);
    pnl_vect_free(&deltaDev);
	pnl_vect_free(&spot);
    pnl_vect_free(&sigma);
	pnl_vect_free(&lambdas);
	pnl_vect_free(&trend); 
	pnl_rng_free(&rng);
	delete P;

	return 0;
}

