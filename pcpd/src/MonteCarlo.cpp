//
// Created by Axel on 12/09/22.
//

#include "MonteCarlo.hpp"
#include "pnl/pnl_matrix.h"
#include <math.h>
#include <iostream>

MonteCarlo::MonteCarlo(BlackScholesModel* mod, Option* opt, PnlRng* rng, double fdStep, long nbSamples)
{
    // Attributs publiques propre au domaine
    mod_ = mod;
    opt_ = opt;
    rng_ = rng;
    fdStep_ = fdStep;
    nbSamples_ = nbSamples;
    path_ = pnl_mat_create(opt_->nbTimeSteps_ + 1 ,mod_->size_);
    shiftPath_ = pnl_mat_create(opt_->nbTimeSteps_ + 1 ,mod_->size_);

    // Attributs privés pour l'optimisation
    delta_vect_util = new double[opt_->size_];
    delta_carre_vect_util = new double[opt_->size_];
}

MonteCarlo::~MonteCarlo(){
    // Attributs publiques propre au domaine
    pnl_mat_free(&path_);
    pnl_mat_free(&shiftPath_);

    // Attributs privés pour l'optimisation
    delete(delta_vect_util);
    delete(delta_carre_vect_util);
}

void MonteCarlo::price(double& prix, double& std_dev)
{
    double esp = 0;
    double esp2 = 0;

    for(int j = 0; j < nbSamples_; j++)
    {
        mod_->asset(path_, opt_->T_, opt_->nbTimeSteps_, rng_);
        double payoff = opt_->payoff(path_);
        esp += payoff;
        esp2 += payoff * payoff;
    }

    esp /= nbSamples_;
    esp2 /= nbSamples_;

    prix = exp(-mod_->r_ * opt_->T_) * esp;
    std_dev = sqrt((exp(-2 * mod_->r_ * opt_->T_) * esp2 - prix * prix) / nbSamples_);
}

void MonteCarlo::price(const PnlMat* past, double t, double& prix, double& std_dev)
{
    double esp = 0; // E[X]
    double esp2 = 0; // Esperance des carrés des prix cad E[X^2]

    for(int j = 0; j < nbSamples_; j++)
    {
        mod_->asset(path_, t, opt_->T_, opt_->nbTimeSteps_, rng_, past);
        double payoff = opt_->payoff(path_);
        esp += payoff;
        esp2 += payoff * payoff;
    }

    esp /= nbSamples_;
    esp2 /= nbSamples_;

    prix = exp(-mod_->r_ * (opt_->T_ - t)) * esp;
    std_dev = sqrt((exp(-2 * mod_->r_ * (opt_->T_ - t)) * esp2 - prix * prix) / nbSamples_);
}

void MonteCarlo::delta(const PnlMat* past, double t, PnlVect* delta, PnlVect* std_dev)
{
     double delta_d_tmp, timestep = opt_->T_/opt_->nbTimeSteps_;
     double const esperanceConstante = exp(-mod_->r_ * (opt_->T_ - t)) / (2 * fdStep_ * nbSamples_);
     double const varianceConstante = esperanceConstante * esperanceConstante;

     // Mise à zero des valeurs du tableau
     for (int d = 0; d < opt_->size_; d++){
         delta_vect_util[d] = 0;
         delta_carre_vect_util[d] = 0;
     }

     // Parcours des pas de temps
     for(int j=0; j<nbSamples_; j++){
         // Génèse d'une trajectoire
         mod_->asset(path_, t, opt_->T_, opt_->nbTimeSteps_, rng_, past);
         // Parcours de chaque actif pour calculer son delta
         for (int d = 0; d < opt_->size_; d++){
              //Shifting positif
              mod_->shiftAsset(shiftPath_, path_, d, fdStep_, t, timestep);
              double payoff_sh_plus = opt_->payoff(shiftPath_);
              // Shifting negatif
              mod_->shiftAsset(shiftPath_, path_, d, -fdStep_, t, timestep, false);
              double payoff_sh_neg = opt_->payoff(shiftPath_);
              delta_d_tmp = payoff_sh_plus - payoff_sh_neg;
              delta_vect_util[d] += delta_d_tmp;
              delta_carre_vect_util[d] += delta_d_tmp * delta_d_tmp;
         }
     }
     // Calcul et set des vecteurs
     for (int d = 0; d < opt_->size_; d++){
         // Calcul des deltas
         delta_vect_util[d] *= esperanceConstante / pnl_mat_get(past, past->m - 1, d);
         pnl_vect_set(delta, d, delta_vect_util[d]);

         // Calcul des std_delta
         delta_carre_vect_util[d] *= varianceConstante * nbSamples_ / (pnl_mat_get(past, past->m - 1, d) * pnl_mat_get(past, past->m - 1, d));
         delta_carre_vect_util[d] -= delta_vect_util[d] * delta_vect_util[d];
         pnl_vect_set(std_dev, d, sqrt(delta_carre_vect_util[d]/nbSamples_));
     }

     // Liberation effectuée une seule fois lors la destruction de MonteCarlo
}

void MonteCarlo::delta(PnlVect* delta, PnlVect* std_dev){
    double delta_d_tmp, timestep = opt_->T_/opt_->nbTimeSteps_;
    double const esperanceConstante = exp(-mod_->r_ * opt_->T_) / (2 * fdStep_ * nbSamples_);
    double const varianceConstante = esperanceConstante * esperanceConstante;

    // Mise à zero des valeurs du tableau
    for (int d = 0; d < opt_->size_; d++){
        delta_vect_util[d] = 0;
        delta_carre_vect_util[d] = 0;
    }

    // Parcours des pas de temps
    for(int j=0; j<nbSamples_; j++){
        // Génèse d'une trajectoire
        mod_->asset (path_, opt_->T_, opt_->nbTimeSteps_, rng_);
        // Parcours de chaque actif pour calculer son delta
        for (int d = 0; d < opt_->size_; d++){
            //Shifting positif
            mod_->shiftAsset(shiftPath_, path_, d, fdStep_,0, timestep);
            double payoff_sh_plus = opt_->payoff(shiftPath_);
            // Shifting negatif
            mod_->shiftAsset(shiftPath_, path_, d, -fdStep_, 0, timestep, false);
            double payoff_sh_neg = opt_->payoff(shiftPath_);
            delta_d_tmp = payoff_sh_plus - payoff_sh_neg;
            delta_vect_util[d] += delta_d_tmp;
            delta_carre_vect_util[d] += delta_d_tmp * delta_d_tmp;
        }
    }
    // Calcul et set des vecteurs
    for (int d = 0; d < opt_->size_; d++){
        // Calcul des deltas
        delta_vect_util[d] *= esperanceConstante / pnl_vect_get(mod_->spot_, d);
        pnl_vect_set(delta, d, delta_vect_util[d]);

        // Calcul des std_delta
        delta_carre_vect_util[d] *= varianceConstante * nbSamples_ / (pnl_vect_get(mod_->spot_, d) * pnl_vect_get(mod_->spot_, d));
        delta_carre_vect_util[d] -= delta_vect_util[d] * delta_vect_util[d];
        pnl_vect_set(std_dev, d, sqrt(delta_carre_vect_util[d]/nbSamples_));
    }

    // Liberation effectuée à la destruction de MonteCarlo

}

void MonteCarlo::price_and_delta(double& prix, double& std_dev, PnlVect* delta, PnlVect* std_delta_dev){
    // variable pour Price
    double esp = 0, esp2 = 0;
    // variable pour delta
    double delta_d_tmp, timestep = opt_->T_/opt_->nbTimeSteps_;
    double const esperanceConstante = exp(-mod_->r_ * opt_->T_) / (2 * fdStep_ * nbSamples_);
    double const varianceConstante = esperanceConstante * esperanceConstante;

    // Mise à zero des valeurs des tableaux utilitaires des deltas
    for (int d = 0; d < opt_->size_; d++){
        delta_vect_util[d] = 0;
        delta_carre_vect_util[d] = 0;
    }

    // Parcours des pas de temps
    for(int j=0; j<nbSamples_; j++){
        // Génèse d'une trajectoire pour le prix et les deltas
        mod_->asset (path_, opt_->T_, opt_->nbTimeSteps_, rng_);

        // /!\ Etape Price
        double payoff = opt_->payoff(path_);
        esp += payoff;
        esp2 += payoff * payoff;

        // /!\ Etape Delta
        // Parcours de chaque actif pour calculer son delta
        for (int d = 0; d < opt_->size_; d++){
            //Shifting positif
            mod_->shiftAsset(shiftPath_, path_, d, fdStep_,0, timestep);
            double payoff_sh_plus = opt_->payoff(shiftPath_);
            // Shifting negatif
            mod_->shiftAsset(shiftPath_, path_, d, -fdStep_, 0, timestep, false);
            double payoff_sh_neg = opt_->payoff(shiftPath_);
            delta_d_tmp = payoff_sh_plus - payoff_sh_neg;
            delta_vect_util[d] += delta_d_tmp;
            delta_carre_vect_util[d] += delta_d_tmp * delta_d_tmp;
        }
    }

    // /!\ Etape Price
    esp /= nbSamples_;
    esp2 /= nbSamples_;
    prix = exp(-mod_->r_ * opt_->T_) * esp;
    std_dev = sqrt((exp(-2 * mod_->r_ * opt_->T_) * esp2 - prix * prix) / nbSamples_);

    // /!\ Etape Delta
    // Calcul et set des vecteurs
    for (int d = 0; d < opt_->size_; d++){
        // Calcul des deltas
        delta_vect_util[d] *= esperanceConstante / pnl_vect_get(mod_->spot_, d);
        pnl_vect_set(delta, d, delta_vect_util[d]);

        // Calcul des std_delta
        delta_carre_vect_util[d] *= varianceConstante * nbSamples_ / (pnl_vect_get(mod_->spot_, d) * pnl_vect_get(mod_->spot_, d));
        delta_carre_vect_util[d] -= delta_vect_util[d] * delta_vect_util[d];
        pnl_vect_set(std_delta_dev, d, sqrt(delta_carre_vect_util[d]/nbSamples_));
    }

}

void MonteCarlo::price_and_delta(const PnlMat* past, double t, double& prix, double& std_dev, PnlVect* delta, PnlVect* std_delta_dev){
    // Variables pour Price
    double esp = 0, esp2 = 0; // E[X] et E[X^2]
    // Variables pour deltas
    double delta_d_tmp, timestep = opt_->T_/opt_->nbTimeSteps_;
    double const esperanceConstante = exp(-mod_->r_ * (opt_->T_ - t)) / (2 * fdStep_ * nbSamples_);
    double const varianceConstante = esperanceConstante * esperanceConstante;

    // Mise à zero des valeurs du tableau
    for (int d = 0; d < opt_->size_; d++){
        delta_vect_util[d] = 0;
        delta_carre_vect_util[d] = 0;
    }

    // Parcours des pas de temps
    for(int j=0; j<nbSamples_; j++){
        // Génèse d'une trajectoire
        mod_->asset(path_, t, opt_->T_, opt_->nbTimeSteps_, rng_, past);
        // /!\ Etape Price
        double payoff = opt_->payoff(path_);
        esp += payoff;
        esp2 += payoff * payoff;
        // /!\ Etape Deltas
        // Parcours de chaque actif pour calculer son delta
        for (int d = 0; d < opt_->size_; d++){
            //Shifting positif
            mod_->shiftAsset(shiftPath_, path_, d, fdStep_, t, timestep);
            double payoff_sh_plus = opt_->payoff(shiftPath_);
            // Shifting negatif
            mod_->shiftAsset(shiftPath_, path_, d, -fdStep_, t, timestep, false);
            double payoff_sh_neg = opt_->payoff(shiftPath_);
            delta_d_tmp = payoff_sh_plus - payoff_sh_neg;
            delta_vect_util[d] += delta_d_tmp;
            delta_carre_vect_util[d] += delta_d_tmp * delta_d_tmp;
        }
    }

    // /!\ Etape Price
    esp /= nbSamples_;
    esp2 /= nbSamples_;
    prix = exp(-mod_->r_ * (opt_->T_ - t)) * esp;
    std_dev = sqrt((exp(-2 * mod_->r_ * (opt_->T_ - t)) * esp2 - prix * prix) / nbSamples_);

    // /!\ Etape Delta
    // Calcul et set des vecteurs
    for (int d = 0; d < opt_->size_; d++){
        // Calcul des deltas
        delta_vect_util[d] *= esperanceConstante / pnl_mat_get(past, past->m - 1, d);
        pnl_vect_set(delta, d, delta_vect_util[d]);

        // Calcul des std_delta
        delta_carre_vect_util[d] *= varianceConstante * nbSamples_ / (pnl_mat_get(past, past->m - 1, d) * pnl_mat_get(past, past->m - 1, d));
        delta_carre_vect_util[d] -= delta_vect_util[d] * delta_vect_util[d];
        pnl_vect_set(std_delta_dev, d, sqrt(delta_carre_vect_util[d]/nbSamples_));
    }

    // Liberation effectuée une seule fois lors la destruction de MonteCarlo
};

// Renvoie ti tel que ti = max{tk <= tau ; k}
double MonteCarlo::nearest_ti(double tau)
{
    if(tau == 0)
    {
        return 0;
    }

    double ti = 0;
    double step = opt_->T_/opt_->nbTimeSteps_;

    for(ti = 0; ti <= tau; ti+=step){continue;}

    return ti - step;
    
}

// renvoie le i d'un ti
int MonteCarlo::geti(double ti)
{
    return (int) (ti * opt_->nbTimeSteps_/opt_->T_);
}

PnlMat* MonteCarlo::extract_past(const PnlMat* path, int H, double tau)
{
    PnlMat* past;
    double ti = nearest_ti(tau);
    double i = geti(ti);
    PnlVect* row_j = pnl_vect_create(path->n);

    if(ti == tau)
    {
        past = pnl_mat_create(i + 1, path->n);
    }
    else
    {
        past = pnl_mat_create(i + 2, path->n);
        int indice = (int)(tau * H / opt_->T_);
        pnl_mat_get_row(row_j, path, indice);
        pnl_mat_set_row(past, row_j, i + 1);
    }

    int j = 0; // index de parcours de past
    for(int k = 0; k < path->m; k++) // parcours de path
    {
        if(k * opt_->T_/H > tau){break;}

        if(j * opt_->T_/opt_->nbTimeSteps_ == k * opt_->T_/H)
        {
            pnl_mat_get_row(row_j, path, k);
            pnl_mat_set_row(past, row_j, j);
            j++;
        }
    }

    pnl_vect_free(&row_j);

    return past;
}

double MonteCarlo::calculate_p_and_l(const PnlMat* path, int H){
    double step = opt_->T_ / H;
    double p0, std_price;


    PnlVect* std_delta = pnl_vect_create(mod_->size_);
    PnlVect* previous_delta = pnl_vect_create(mod_->size_);
    PnlVect* current_delta = pnl_vect_create(mod_->size_);
    price_and_delta(p0, std_price, previous_delta, std_delta);
    PnlMat* past;

    double V = p0 - pnl_vect_scalar_prod(previous_delta, mod_->spot_);

    for (double tau=step; tau<= opt_->T_; tau+=step){
        past = extract_past(path, H, tau);
        double expo = exp(mod_->r_ * opt_->T_ / H);
        delta(past, tau, current_delta, std_delta);
        PnlVect S_tau = pnl_vect_wrap_mat_row(past, past->m - 1);
        pnl_vect_minus_vect(previous_delta, current_delta);

        V = V * expo + pnl_vect_scalar_prod(previous_delta, &S_tau);

        pnl_vect_free(&previous_delta);
        previous_delta = pnl_vect_copy(current_delta);
        pnl_mat_free(&past);
    }

    PnlVect S_H = pnl_vect_wrap_mat_row(path, path->m - 1);
    past = extract_past(path, H, opt_->T_);
    double res = V + pnl_vect_scalar_prod(current_delta, &S_H) - opt_->payoff(past);
    
    pnl_mat_free(&past);
    pnl_vect_free(&std_delta);
    pnl_vect_free(&previous_delta);
    pnl_vect_free(&current_delta); 
    return res;
}