#include "testCall.hpp"

using namespace Couverture;


/*--------------------------------------------------------*/
	void extract_past_from_mat ( PnlMat* past, PnlMat* assetPrices, PnlVect* sti, int sizeOfpast, int nbAssets, int rapport ) {
		PnlVect* lignei = pnl_vect_create(nbAssets);
		for (int i = 0; i < sizeOfpast-1; i++) {	
			pnl_mat_get_row ( lignei, assetPrices,  i * rapport);
			pnl_mat_set_row (past, lignei, i);		
		}
		//pnl_vect_free(&lignei);
		pnl_mat_set_row (past, sti, sizeOfpast-1);		
	}

	// // pnl
	double  Pnl( int size, double M, double h, double H, double N , double T, double strike, double r, double rho, PnlVect* trend,  PnlVect* sigma, PnlVect *lambdas, PnlVect *spots, PnlMat* assetPrices) {

		BlackScholesModel blackScholesModel(size, r, rho, trend, sigma, spots);
		PnlRng* rng = pnl_rng_create(PNL_RNG_MERSENNE);
		pnl_rng_sseed(rng, time(NULL));
		AsianOption asianOption(T, N, size, lambdas, spots, strike);
		MonteCarlo monteCarlo(&blackScholesModel, &asianOption, rng, h,  M );
		//monteCarlo.Pnl(assetPrices, pnl, price0, stdDev,  H);

		int nbAssets = 1;
		/*double T = opt_->T_;
		double N = opt_->nbTimeSteps_;*/

		double factor = exp(r* T/ H);
		double stept = T/N; 
		double stepTau = T/H;
		int rapport = H/N;
		double stiDelta = 0;

		double deltaPrec;
        double price0;
		double price;

		double std_dev;

        double pnl = 0;
		PnlVect* delta = pnl_vect_create(size);
		PnlVect* deltaDev;

		// en t = 0
		// price(price0, std_dev);
		// delta(deltaPrec, std_dev_vect);
		//priceDelta(NULL, 0.0, deltaPrec, std_dev_vect, price0, std_dev );
        pnl_cf_call_bs(pnl_mat_get(assetPrices, 0, 0), strike, T, r, 0.0, pnl_vect_get(sigma, 0), &price0, &deltaPrec );
		cout <<" price by pnl : " << price0<<endl;
		PnlMat* past = pnl_mat_create(1, size);
		PnlVect* thespots = pnl_vect_create(size);

		pnl_mat_get_row (thespots, assetPrices, 0);

        pnl_mat_set_row(past, thespots, 0);

		monteCarlo.priceDelta( past , 0.0,  delta, deltaDev, price,  std_dev );
		cout <<"my price: " << price0<<endl;

		double V = price0 - deltaPrec * pnl_mat_get(assetPrices, 0, 0); // v en 0
		cout <<"vo :"<< V<< endl;

		PnlVect* sti = pnl_vect_create(nbAssets);
		double deltai;
        double pricet;
		for (int i = 0 ; i < N ; i++) {
				double t = i * stept;
				double sizeOfMatPast = i + 2;
				PnlMat* past = pnl_mat_create(sizeOfMatPast , nbAssets);
				for (int j = 1; j < rapport + 1; j++) {
					double taui = t + j * stepTau;
					// extraction de past
					pnl_mat_get_row(sti, assetPrices, i * rapport + j);
					extract_past_from_mat (past, assetPrices, sti, sizeOfMatPast, nbAssets, rapport);
					pnl_cf_call_bs(pnl_mat_get(past, past->m - 1, 0),strike, T, r, 0.0, pnl_vect_get(sigma, 0), &pricet, &deltai);
					V = V*factor - (deltai - deltaPrec) * pnl_mat_get(past, past->m - 1, 0);
					//cout <<i << "  "<< j<< " V: "<< V<< "   stiD: "<<stiDelta << endl;
					deltaPrec = deltai;	
				}
				pnl_mat_free(&past);
		}
		pnl = V + deltai*pnl_mat_get(assetPrices, assetPrices->m - 1, 0) - (max(0.0, pnl_mat_get(assetPrices, assetPrices->m - 1, 0) - strike));
        return pnl;	
    }


/*--------------------------------------------------------*/


int main(int argc, char** argv)
{ 
	if (argc != 3) {
		 cout << "two file needed" << endl;
		 return 0;
	}
	string key;
	double T, r, strike;
    PnlVect *spot, *sigma, *lambdas, *trend;
	
    string type;
    int size, nbTimeSteps, H;
    size_t n_samples;
	char* marketFile = argv[1];
	char* dataInput = argv[2];
	double rho;	
	size_t M ;
	PnlRng* rng = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	// read params
	Param* P = new Parser(dataInput);
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
	P->extract("hedging dates number", H);
	P->extract("trend", trend, size);
    P->extract("strike", strike);

	// hedge data
	PnlMat* assetPrices = pnl_mat_create_from_file(marketFile);

	double price0=0;
	double stdDev=0;
	double h = 0.1;
/**************************/

	double pnl = Pnl(size, M, h , H,  nbTimeSteps ,  T,  strike,  r, rho, trend, sigma, lambdas, spot, assetPrices);
    cout << pnl << endl;

/****************************/




	// pnl_mat_free(&assetPrices);
	// pnl_vect_free(&spot);
    // pnl_vect_free(&sigma);
	// pnl_vect_free(&lambdas);

	delete P;
	return 0;
}
