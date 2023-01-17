#include "BlackScholesModel.hpp"
#include <math.h>
#include <iostream>

BlackScholesModel::BlackScholesModel(int size, double r, PnlMat* L, PnlVect* sigma, PnlVect* spot){
    size_ = size;
    r_ = r;
    L_ = pnl_mat_copy(L);
    sigma_ = pnl_vect_copy(sigma);
    spot_ = pnl_vect_copy(spot);
    trend_ = nullptr;
    rho_ = 0.0;
}

BlackScholesModel::BlackScholesModel(int size, double r, double rho, PnlVect* sigma, PnlVect* spot){
    size_ = size;
    r_ = r;
    trend_ = nullptr;
    rho_ =  rho;
    spot_ = pnl_vect_copy(spot);
    sigma_ = pnl_vect_copy(sigma);
    L_ = compute_cholesky();
    gaussian_simulation_ = pnl_vect_create(size_);
};

BlackScholesModel::BlackScholesModel(int size, double r, double rho, PnlVect* sigma, PnlVect* spot, PnlVect* trend){
    size_ = size;
    r_ = r;
    rho_ =  rho;
    spot_ = pnl_vect_copy(spot);
    sigma_ = pnl_vect_copy(sigma);
    L_ = compute_cholesky();
    gaussian_simulation_ = pnl_vect_create(size_);
    trend_ = pnl_vect_copy(trend);
};

PnlMat* BlackScholesModel::compute_cholesky(){
    PnlMat *L = pnl_mat_create_from_scalar(size_, size_, rho_);
    for(int i=0; i < L->m; i++){
        pnl_mat_set_diag(L, 1, i);
    }
    pnl_mat_chol(L);

    return L;
}

BlackScholesModel::~BlackScholesModel()
{
    pnl_mat_free(&L_);
    pnl_vect_free(&spot_);
    pnl_vect_free(&sigma_);
    pnl_vect_free(&gaussian_simulation_);
    if (trend_ != nullptr){
        pnl_vect_free(&trend_);
    }
}

void BlackScholesModel::asset (PnlMat* path, double T, int nbTimeSteps, PnlRng* rng){
    //remplissage de la première ligne du path avec les prix spots
    pnl_mat_set_row(path, spot_, 0);

    double pas = T/nbTimeSteps;
    // Parcours selon les lignes de la matrices
    for (int i = 1; i <= nbTimeSteps; i++){
        asset_at_time_i(i, path, pas, rng);
    }
}

void BlackScholesModel::asset_at_time_i(int i, PnlMat* path, double pas, PnlRng* rng, int offset)
{
    // Simulation du vecteur gaussien
    pnl_vect_rng_normal(gaussian_simulation_, size_, rng);
    double exp_contain, res;
    for(int d = 0; d < size_; d++){
        PnlVect l_d = pnl_vect_wrap_mat_row(L_, d);
        double vol = pnl_vect_get(sigma_, d);

        exp_contain = (r_ - (vol * vol)/2) * pas;
        exp_contain += vol * sqrt(pas) * pnl_vect_scalar_prod(&l_d, gaussian_simulation_);
        res = pnl_mat_get(path, i-1 + offset , d) * exp(exp_contain);
        pnl_mat_set(path, i, d, res);
    }
}

void BlackScholesModel::asset(PnlMat* path, double t, double T, int nbTimeSteps, PnlRng* rng, const PnlMat* past){

    pnl_mat_set_subblock(path, past, 0, 0);

    if(t == T){return;}

    double k = t * nbTimeSteps/T;
    int k_c = (int) k; // k caster en int pour comparer les dates de constatation
    double t_ip1 = (k_c + 1) * T/nbTimeSteps;
    
    int start_index = past->m - 1;  // Cas t != date de constatation
    int offset = 1;
    if(k - k_c == 0) // t est une date de constatation
    {
        start_index++;
        offset = 0;
    }

    // Simulation en i = start_index:

    double pas = t_ip1 - t;
    asset_at_time_i(start_index, path, pas, rng, offset);
    //std::cout << "Last prices: " << pnl_mat_get(past,past->m - 1, 0) << std::endl;

    // Simulation en i > start_index:

    pas = T/nbTimeSteps;
    for (int i = start_index + 1; i <= nbTimeSteps; i++){
        asset_at_time_i(i, path, pas, rng);
        //std::cout << "Last prices: " << pnl_mat_get(past,past->m - 1, 0) << std::endl;
    }


}

void BlackScholesModel::shiftAsset(PnlMat* shift_path, const PnlMat* path, int d, double h, double t, double timestep, bool set_subblock)
{
    if (set_subblock){
        pnl_mat_set_subblock(shift_path, path, 0, 0);
    }

    double k = t/timestep;
    int k_c = (int)k; // k caster en int pour comparer les dates de constatation

    int start_index;
    start_index = (k -k_c) == 0 ? k : k + 1;




    //Shift de l'actif d
    for (int i = start_index; i < path->m; i++)
    {
        double val = pnl_mat_get(path, i, d)*(1 + h);
        pnl_mat_set(shift_path, i, d, val);
    }
    
    
}

void BlackScholesModel::simul_market(PnlMat* simulatedMarket, double H, double T, PnlRng* rng){
    if (trend_ == nullptr){
        throw std::invalid_argument("Trend is not initialized! ");
    }
    //remplissage de la première ligne du path avec les prix spots
    pnl_mat_set_row(simulatedMarket, spot_, 0);

    double pas = T/H;
    // Parcours selon les lignes de la matrices
    for (int i= 1; i<= H; i++){
        simulate_at_time_i(i, simulatedMarket, pas, rng);
    }
}

void BlackScholesModel::simulate_at_time_i(int i, PnlMat* simulatedMarket, double pas, PnlRng* rng)
{
    // Simulation du vecteur gaussien
    pnl_vect_rng_normal(gaussian_simulation_, size_, rng);
    double exp_contain, res;
    for(int d = 0; d < size_; d++){
        PnlVect l_d = pnl_vect_wrap_mat_row(L_, d);
        double vol = pnl_vect_get(sigma_, d);

        exp_contain = (pnl_vect_get(trend_, d) - (vol * vol)/2) * pas;
        exp_contain += vol * sqrt(pas) * pnl_vect_scalar_prod(&l_d, gaussian_simulation_);
        res = pnl_mat_get(simulatedMarket, i-1, d) * exp(exp_contain);
        pnl_mat_set(simulatedMarket, i, d, res);
    }
}

