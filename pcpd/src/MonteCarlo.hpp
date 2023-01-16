#pragma once

#include "Option.hpp"
#include "BlackScholesModel.hpp"
#include "pnl/pnl_random.h"

namespace Couverture {
  class MonteCarlo
  {
    public:
      MonteCarlo(BlackScholesModel* mod_, Option* opt_, PnlRng* rng_, double fdStep_, long nbSamples_);
      BlackScholesModel* mod_; /*! pointeur vers le modèle */
      Option* opt_;            /*! pointeur sur l'option */
      PnlRng* rng_;            /*! pointeur sur le générateur */
      double fdStep_;          /*! pas de différence finie */
      long nbSamples_;         /*! nombre de tirages Monte Carlo */
      

      /**
       * Calcule le prix de l'option à la date 0
       *
       * @param[out] prix valeur de l'estimateur Monte Carlo
       * @param[out] ic écart type de l'estimateur
       */
      void price(double& prix, double& std_dev);

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
       * Calcule le delta de l'option à la date 0
       *
       * @param[in] t date à laquelle le calcul est fait
       * @param[out] delta contient le vecteur de delta
       * @param[out] std_dev contient l'écart type de l'estimateur
       */
      void delta(PnlVect* delta, PnlVect* std_dev);
	  void priceDelta(PnlMat* past, double t, PnlVect* delta, PnlVect* deltaDev, double& prix, double& std_dev) ;
    void deltaOnly(const PnlMat* past, double t, PnlVect* delta);

	  void extract_past_from_mat ( PnlMat* past, PnlMat* assetPrices, PnlVect* sti, int sizeOfpast, int nbAssets, int rapport );

	  void Pnl(PnlMat* assetPrices, double &pnl, double &price0, double &std_dev, double H );
	void Pnl2 (PnlMat* assetPrices, double &pnl, double &price0, double &std_dev, double H );

  };
}