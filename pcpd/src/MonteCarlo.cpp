#include <iostream>
#include "MonteCarlo.hpp"
#include <cmath>
using namespace std;
namespace Couverture {

	MonteCarlo::MonteCarlo (BlackScholesModel* mod_, Option* opt_, PnlRng* rng_, double fdStep_, long nbSamples_) {
		this->mod_ = mod_;
		this->opt_ = opt_;
		this->rng_ = rng_;
		this->fdStep_ = fdStep_;
		this->nbSamples_ = nbSamples_;
	}

	// compute price of option at t = 0
	void MonteCarlo::price(double& prix, double& std_dev){
		// options params
		long nbSamples = nbSamples_;
		double maturity = opt_->T_;
		int nbTimeSteps = opt_->nbTimeSteps_;
		double r = mod_->r_;
		int nbAssets = opt_->size_;

		
		double sommePayoff = 0;
		double sommePayoff2 = 0;
		PnlMat* assetPrices = pnl_mat_create(nbTimeSteps + 1, nbAssets);
		for (long i = 0; i < nbSamples; i++) {
			// price
			mod_->asset(assetPrices, maturity, nbTimeSteps, rng_);
			double payoffComputed = opt_->payoff(assetPrices);
			sommePayoff += payoffComputed;
			// variance
			sommePayoff2 += payoffComputed*payoffComputed;
		}

		double variance = exp(-2*r*maturity) * ((sommePayoff2 / nbSamples)  - (sommePayoff/nbSamples)*(sommePayoff/nbSamples));
		prix = exp(-r*maturity) * (sommePayoff / nbSamples);
		std_dev = sqrt(variance)/sqrt(nbSamples);
		pnl_mat_free(&assetPrices);
	}


	// compute price of option at t > 0
	void MonteCarlo::price(const PnlMat* past, double t, double& prix, double& std_dev){

		// options params
		long nbSamples = nbSamples_;
		double maturity = opt_->T_;
		int nbTimeSteps = opt_->nbTimeSteps_;
		int r = mod_->r_;
		int nbAssets = opt_->size_;
		int nbStep = maturity / fdStep_;
	
		double sommePayoff = 0;
		double sommePayoff2 = 0;
		double payoffComputed = 0;
		PnlMat* assetPrices = pnl_mat_create(nbTimeSteps + 1, nbAssets);
		for (long i = 0; i < nbSamples; i++) {
			// price
			mod_->asset(assetPrices, t, maturity, nbTimeSteps, rng_, past);
			payoffComputed = opt_->payoff(assetPrices);
			sommePayoff += payoffComputed;
			// variance
			sommePayoff2 += payoffComputed*payoffComputed;
		}

		double variance = exp(-2*r*maturity) * (sommePayoff2 / nbSamples)  - (sommePayoff/nbSamples)*(sommePayoff/nbSamples);
		prix = exp(-r* (maturity - t) ) * (sommePayoff / nbSamples);
		std_dev = sqrt(variance)/sqrt(nbSamples);
		pnl_mat_free(&assetPrices);
	}

		// t  = 0
	void MonteCarlo::delta(PnlVect* delta, PnlVect* std_dev) {		
		PnlMat* past = pnl_mat_create(1, opt_->size_);
        pnl_mat_set_row(past, mod_->spot_, 0);
		MonteCarlo::delta(past, 0.0, delta, std_dev);
		pnl_mat_free(&past);
	}

	// t > 0
	void MonteCarlo::delta(const PnlMat* past, double t, PnlVect* delta, PnlVect* std_dev) {

		int nbAssets = opt_->size_;
		double maturity = opt_->T_;
		int nbTimeSteps = opt_->nbTimeSteps_;
		double r = mod_->r_;
		double timestep = maturity / nbTimeSteps;
		double factor = exp(-r*(maturity-t))/(2*nbSamples_*fdStep_);

		// les deltas_dev
		PnlVect* somme = pnl_vect_create(nbAssets); // A LIBERER
		PnlVect* somme2 = pnl_vect_create(nbAssets);

		PnlMat* assetPrices = pnl_mat_create(nbTimeSteps + 1, nbAssets);
		PnlMat* pricesShifted = pnl_mat_create(nbTimeSteps + 1, nbAssets);
		for (int i = 0; i <nbSamples_; i++){
			mod_->asset(assetPrices, t, maturity, nbTimeSteps, rng_, past);
			for (int d = 0; d<nbAssets;  d++) {
				mod_->shiftAsset(pricesShifted, assetPrices, d, fdStep_, t, timestep);
				double payoffComputed =opt_->payoff(pricesShifted);
				mod_->shiftAsset(pricesShifted, assetPrices, d, -fdStep_, t, timestep);
				payoffComputed -=opt_->payoff(pricesShifted);
				pnl_vect_set(delta, d, pnl_vect_get(delta, d) + payoffComputed);
				// delta
				pnl_vect_set(somme, d, pnl_vect_get(somme, d) +   payoffComputed); 
				pnl_vect_set(somme2, d, pnl_vect_get(somme2, d) +   payoffComputed*payoffComputed);
			}
		}	
		PnlVect* last_row = pnl_vect_create(nbAssets);
		pnl_mat_get_row(last_row, past, past->m-1);
		double exp2rT = exp(-2*r*maturity);
		for (int d =0; d<nbAssets;  d++) {
			double st = pnl_vect_get(last_row, d);
			pnl_vect_set(delta, d, (pnl_vect_get(delta, d)*factor)/ st);
			// delta
			double factor = 1/(2*st*fdStep_);
			double variance = exp2rT*exp(2*r*t ) *( (factor*factor/nbSamples_)*pnl_vect_get(somme2, d) - pnl_pow_i(( (factor/nbSamples_)*pnl_vect_get(somme, d) ),2) );
			pnl_vect_set( std_dev, d, sqrt(variance/nbSamples_) );
		}

		// memoire
		pnl_vect_free(&somme);
		pnl_vect_free(&somme2);
		pnl_vect_free(&last_row);
		pnl_mat_free(&assetPrices);
	}

	void MonteCarlo::deltaOnly(const PnlMat* past, double t, PnlVect* delta) {
		
		int nbAssets = opt_->size_;
		double maturity = opt_->T_;
		int nbTimeSteps = opt_->nbTimeSteps_;
		double r = mod_->r_;
		double timestep = maturity / nbTimeSteps;
		double factor = exp(-r*(maturity-t))/(2*nbSamples_*fdStep_);

		// les deltas_dev
		PnlMat* assetPrices = pnl_mat_create(nbTimeSteps + 1, nbAssets);
		PnlMat* pricesShifted = pnl_mat_create(nbTimeSteps + 1, nbAssets);
		for (int i = 0; i < nbSamples_; i++){ // nbSamples_
			PnlVect* payoffsComputed = pnl_vect_create(nbAssets);
			mod_->asset(assetPrices, t, maturity, nbTimeSteps, rng_, past);
			for (int d = 0; d<nbAssets;  d++) {
				mod_->shiftAsset(pricesShifted, assetPrices, d, fdStep_, t, timestep);
				double payoffComputed =opt_->payoff(pricesShifted);
				mod_->shiftAsset(pricesShifted, assetPrices, d, -fdStep_, t, timestep);
			// cout << "assets " <<endl;
			// pnl_mat_print(assetPrices);
				payoffComputed -=opt_->payoff(pricesShifted);
				pnl_vect_set(delta, d, pnl_vect_get(delta, d) + payoffComputed);
				// delta
			}
		}	
		PnlVect* last_row = pnl_vect_create(nbAssets);
		pnl_mat_get_row(last_row, past, past->m-1);
		double exp2rT = exp(-2*r*maturity);
		for (int d =0; d<nbAssets;  d++) {
			double st = pnl_vect_get(last_row, d);
			pnl_vect_set(delta, d, (pnl_vect_get(delta, d)*factor)/ st);
			}
		// memoire
		pnl_vect_free(&last_row);
		pnl_mat_free(&assetPrices);
	}


	void MonteCarlo::priceDelta(PnlMat* past, double t, PnlVect* delta, PnlVect* deltaDev, double& prix, double& std_dev) {

		// options params
		long nbSamples = nbSamples_;
		double maturity = opt_->T_;
		int nbTimeSteps = opt_->nbTimeSteps_;
		double r = mod_->r_;
		int nbAssets = opt_->size_;
		int nbStep = maturity / fdStep_;

		PnlVect* last_row = NULL;

		if (t==0.0) { // A VOIR
			past = pnl_mat_create(1, nbAssets);
        	pnl_mat_set_row(past, mod_->spot_, 0);
			last_row = pnl_vect_create(nbAssets);
		}
		
		// for delta
		double timestep = maturity / nbTimeSteps;
		double factor = exp(-r*(maturity-t))/(2*nbSamples_*fdStep_);

		double sommePayoff = 0;
		double sommePayoff2 = 0;
		double payoffComputed = 0;

		// les deltas_dev
		PnlVect* somme2 = pnl_vect_create(nbAssets); 

		PnlMat* pricesShifted = pnl_mat_create(nbTimeSteps + 1, nbAssets);
		PnlMat* assetPrices = pnl_mat_create(nbTimeSteps + 1, nbAssets);
		for (long i = 0; i < nbSamples; i++) {
			PnlVect* payoffsComputed = pnl_vect_create(nbAssets);
			// price
			mod_->asset(assetPrices, t, maturity, nbTimeSteps, rng_, past);
			payoffComputed = opt_->payoff(assetPrices);
			sommePayoff += payoffComputed;
			// variance
			sommePayoff2 += payoffComputed*payoffComputed;

			for (int d = 0; d<nbAssets;  d++) {
				mod_->shiftAsset(pricesShifted, assetPrices, d, fdStep_, t, timestep);
				double payoffComputed = opt_->payoff(pricesShifted);
				mod_->shiftAsset(pricesShifted, assetPrices, d, -fdStep_, t, timestep);
				payoffComputed -= opt_->payoff(pricesShifted);
				pnl_vect_set(payoffsComputed, d, payoffComputed);
			}
			pnl_vect_plus_vect(delta, payoffsComputed);
			pnl_vect_mult_vect_term(payoffsComputed, payoffsComputed);
			pnl_vect_plus_vect(somme2, payoffsComputed);
			pnl_vect_free(&payoffsComputed);
		}

		double exp2rT = exp(-2*r*maturity);
		pnl_mat_get_row(last_row, past, past->m-1);	
		for (int d =0; d<nbAssets;  d++) {
			double st = pnl_vect_get(last_row, d);
			// delta
			double factord = 1/(2*st*fdStep_);
			double variance = exp2rT*exp(2*r*t ) *( (factord*factord/nbSamples_)*pnl_vect_get(somme2, d) - pnl_pow_i(( (factord/nbSamples_)*pnl_vect_get(delta, d) ),2) );
			// delta dev
			pnl_vect_set( deltaDev, d, sqrt(variance/nbSamples_) );
			// delta
			pnl_vect_set(delta, d, (pnl_vect_get(delta, d)*factor)/ st);
		}
		double variance_prix = exp(-2*r*maturity) * ( (sommePayoff2 / nbSamples)  - (sommePayoff/nbSamples)*(sommePayoff/nbSamples) ) ;
		prix = exp(-r* (maturity - t) ) * (sommePayoff / nbSamples);
		std_dev = sqrt(variance_prix)/sqrt(nbSamples);
		
		pnl_vect_free(&somme2);
		pnl_mat_free(&assetPrices);
		pnl_mat_free(&pricesShifted);
		if (t == 0.0) {
			pnl_mat_free(&past);
			pnl_vect_free(&last_row);
		}

	}

	// // pnl
	void MonteCarlo::Pnl2(PnlMat* assetPrices, double &pnl, double &price0, double &std_dev, double H ) {

		int nbAssets = opt_->size_;
		double T = opt_->T_;
		double factor = exp(mod_->r_* T/ H);
		double N = opt_->nbTimeSteps_;
		double stept = T/N; 
		double stepTau = T/H;
		int rapport = H/N;

		PnlVect* deltaPrec = pnl_vect_create(nbAssets);
		PnlVect* std_dev_vect = pnl_vect_create(nbAssets);

		cout << "delta0 " <<endl;
		pnl_vect_print(deltaPrec);
		cout << "-----------------" <<endl;
		// en t = 0
		priceDelta(NULL, 0.0, deltaPrec, std_dev_vect, price0, std_dev );
		double V = price0 - pnl_vect_scalar_prod(deltaPrec, mod_->spot_); // v en 0
		cout<< " tau = " << 0 << endl;
		pnl_vect_print(deltaPrec);
		cout << "v " << V <<endl;
		cout << "-----------------" <<endl;
		PnlVect* sti = pnl_vect_create(nbAssets);
		PnlVect* deltai = pnl_vect_create_from_scalar(nbAssets, 0);
		PnlVect* deltaMinus = pnl_vect_create(nbAssets);
		//double sizeOfMatPast = 1;
		int compteur = 1 ;
		for (int i = 1 ; i < H+1 ; i++) {    // 1 H+1
				//cout << i << ": " << V << endl;
				double taui = i * stepTau;
				double sizeOfMatPast = (int) floor(taui * N / T) + 1 + ((compteur != rapport ) ? 1 : 0) ;
					PnlMat* past = pnl_mat_create(sizeOfMatPast , nbAssets);
					pnl_mat_get_row(sti, assetPrices, i);
					extract_past_from_mat (past, assetPrices, sti, sizeOfMatPast, nbAssets, rapport );
					deltaOnly(past, taui, deltai);
					pnl_vect_clone( deltaMinus,  deltai);
					pnl_vect_minus_vect(deltaMinus, deltaPrec);
					V = V*factor - pnl_vect_scalar_prod(deltaMinus, sti);
					cout<< " tau = " << taui << endl;
					pnl_vect_print(deltai);
					cout << "v " << V <<endl;

					cout << "-----------------" <<endl;
					pnl_vect_clone( deltaPrec,  deltai);
					pnl_mat_free(&past);
					if (compteur == rapport) {
						compteur = 0;
					}
					compteur ++;
		}
					cout<< " after boucle = " << endl;
					pnl_vect_print(deltai);
					pnl_vect_print(sti);
					cout << "v " << V <<endl;
					cout << "-----------------" <<endl;
					cout << "payoff :" << opt_->payoff(assetPrices) << endl;
					cout << "-----------------" <<endl;
		pnl = V + pnl_vect_scalar_prod(deltai, sti) - opt_->payoff(assetPrices);
		cout << pnl<< endl;
		pnl_vect_free(&std_dev_vect);
		pnl_vect_free(&deltaPrec);
		pnl_vect_free(&sti);
		pnl_vect_free(&deltai);
	}

	// // pnl
	void MonteCarlo::Pnl(PnlMat* assetPrices, double &pnl, double &price0, double &std_dev, double H ) {

		int nbAssets = opt_->size_;
		double T = opt_->T_;
		double factor = exp(mod_->r_* T/ H);
		double N = opt_->nbTimeSteps_;
		double stept = T/N; 
		double stepTau = T/H;

		int rapport = H/N;
		cout << rapport << endl;
		exit(0);

		PnlVect* deltaPrec = pnl_vect_create(nbAssets);
		PnlVect* std_dev_vect = pnl_vect_create(nbAssets);

		// en t = 0
		// price(price0, std_dev);
		// delta(deltaPrec, std_dev_vect);

		priceDelta(NULL, 0.0, deltaPrec, std_dev_vect, price0, std_dev );
		double V = price0 - pnl_vect_scalar_prod(deltaPrec, mod_->spot_); // v en 0
		PnlVect* sti = pnl_vect_create(nbAssets);
		PnlVect* deltai = pnl_vect_create(nbAssets);
		PnlVect* deltaMinus = pnl_vect_create(nbAssets);
		//double sizeOfMatPast = 1;
		for (int i = 1 ; i < 2 ; i++) {    // 0 N
				double t = i * stept;
				double sizeOfMatPast = i + 2 ;
				PnlMat* past = pnl_mat_create(sizeOfMatPast , nbAssets);
				//pnl_mat_resize(past, sizeOfMatPast, nbAssets);
				for (int j = rapport ; j <  rapport + 2; j++) {      // 1 rapport + 1 
					double taui = t + j * stepTau;
					// extraction de past
					pnl_mat_get_row(sti, assetPrices, i * rapport + j);
					extract_past_from_mat (past, assetPrices, sti, sizeOfMatPast, nbAssets, rapport);
					cout << "past " <<endl;
					pnl_mat_print(past);
					cout << "-----------------" <<endl;		
					deltaOnly(past, taui, deltai);
					pnl_vect_clone( deltaMinus,  deltai);
					pnl_vect_minus_vect(deltaMinus, deltaPrec);
					V = V*factor - pnl_vect_scalar_prod(deltaMinus, sti);
					pnl_vect_clone( deltaPrec,  deltai);
				}
				pnl_mat_free(&past);
		}
		return ;
		pnl = V + pnl_vect_scalar_prod(deltaPrec, sti) - opt_->payoff(assetPrices);
		pnl_vect_free(&std_dev_vect);
		pnl_vect_free(&deltaPrec);
		pnl_vect_free(&sti);
		pnl_vect_free(&deltai);
	}


	void MonteCarlo::extract_past_from_mat ( PnlMat* past, PnlMat* assetPrices, PnlVect* sti, int sizeOfpast, int nbAssets, int rapport ) {
		PnlVect* lignei = pnl_vect_create(nbAssets);
		for (int i = 0; i < sizeOfpast-1; i++) {	
			pnl_mat_get_row ( lignei, assetPrices,  i * rapport);
			pnl_mat_set_row (past, lignei, i);		
		}
		pnl_vect_free(&lignei);
		pnl_mat_set_row (past, sti, sizeOfpast-1);		
	}
}