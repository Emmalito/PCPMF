#pragma once

#include "Option.hpp"
#include "BlackScholesModel.hpp"
#include "pnl/pnl_random.h"

class MonteCarlo
{
  private:
    double *delta_vect_util; /// pointeur vers un tableau de stockage de delta pour eviter les surallocations memoires
    double *delta_carre_vect_util; /// pointeur vers un tableau de stockage de delta*delta pour eviter les surallocations memoires

  public:
    BlackScholesModel* mod_; /*! pointeur vers le modèle */
    Option* opt_;            /*! pointeur sur l'option */
    PnlRng* rng_;            /*! pointeur sur le générateur */
    double fdStep_;          /*! pas de différence finie */
    long nbSamples_;         /*! nombre de tirages Monte Carlo */
    PnlMat* path_;
    PnlMat* shiftPath_;


    /**
     * Constructeur
     */
    MonteCarlo(BlackScholesModel* mod, Option* opt, PnlRng* rng, double fdStep, long nbSamples);

    /**
     * Destructerur
     */
     ~MonteCarlo();

    /**
     * Calcule le prix de l'option à la date 0
     *
     * @param[out] prix valeur de l'estimateur Monte Carlo
     * @param[out] ic écart type de l'estimateur
     */
    void price(double& prix, double& std_dev);

    /**
     * Calcule le delta de l'option à la date 0
     *
     * @param[in] t date à laquelle le calcul est fait
     * @param[out] delta contient le vecteur de delta
     * @param[out] std_dev contient l'écart type de l'estimateur
     */
    void delta(PnlVect* delta, PnlVect* std_dev);

    /**
     * Calcule le prix et le delta de l'option à la date 0
     * @param[out] prix valeur de l'estimateur Monte Carlo
     * @param[out] std_dev contient l'écart type de l'estimateur
     * @param[out] delta contient le vecteur de delta
     * @param[out] std_dev contient l'écart type de l'estimateur
     */
    void price_and_delta(double& prix, double& std_dev, PnlVect* delta, PnlVect* std_delta_dev);

    /**
     * Calcule le prix de l'option à la date t
     *
     * @param[in]  past contient la trajectoire du sous-jacent
     * jusqu'à l'instant t
     * @param[in] t date à laquelle le calcul est fait
     * @param[out] prix contient le prix
     * @param[out] std_dev contient l'écart type de l'estimateur
     */
    void price(const PnlMat* past, double t, double& prix, double& std_dev);


    /**
     * Calcule le delta de l'option à la date t
     *
     * @param[in] past contient la trajectoire du sous-jacent
     * jusqu'à l'instant t
     * @param[in] t date à laquelle le calcul est fait
     * @param[out] delta contient le vecteur de delta
     * @param[out] std_dev contient l'écart type de l'estimateur
     */
    void delta(const PnlMat* past, double t, PnlVect* delta, PnlVect* std_dev);

    /**
     * @param[in] past contient la trajectoire du sous-jacent
     * jusqu'à l'instant t
     * @param[in] t date à laquelle le calcul est fait
     * Calcule le prix et le delta de l'option à la date 0
     * @param[out] prix valeur de l'estimateur Monte Carlo
     * @param[out] std_dev contient l'écart type de l'estimateur
     * @param[out] delta contient le vecteur de delta
     * @param[out] std_dev contient l'écart type de l'estimateur
     */
    void price_and_delta(const PnlMat* past, double t, double& prix, double& std_dev, PnlVect* delta, PnlVect* std_delta_dev);

    /**
     * Methode de calcul de la P&L
     */
    double calculate_p_and_l(const PnlMat* path, int H);

    double nearest_ti(double tau);

    int geti(double ti);

    PnlMat* extract_past(const PnlMat* path, int H, double tau);
};
