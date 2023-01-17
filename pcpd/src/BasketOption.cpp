#include "BasketOption.hpp"
#include <stdexcept>
#include <iostream>
#include <algorithm>
BasketOption::BasketOption(double maturity,int nbTimeSteps, int size, double strike, PnlVect * poids){
    if (poids->size != size){
        throw std::invalid_argument("Taille des poids d'entrée different de la taille du modéle ");
     }
    T_ = maturity;
    nbTimeSteps_ = nbTimeSteps;
    size_ = size;
    k_ = strike;
    weights_ = pnl_vect_copy(poids);

}

BasketOption::~BasketOption(){
    pnl_vect_free(&weights_);
}
double BasketOption::payoff(const PnlMat* path){
    PnlVect price_at_maturity = pnl_vect_wrap_mat_row(path, path->m -1);
    double res = pnl_vect_scalar_prod(weights_, &price_at_maturity) - k_;
    return res > 0 ? res : 0;

}