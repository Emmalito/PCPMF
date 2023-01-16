#pragma once

#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"

/// \brief Modèle de Black Scholes
namespace Couverture {
  class BlackScholesModel
  {
    public:
      BlackScholesModel(int size, double r, double rho, PnlVect* trend_, PnlVect* sigma, PnlVect* spot);
      int size_;       /// nombre d'actifs du modèle
      double r_;       /// taux d'intérêt
      PnlMat* chol_corr_;     /// factorisation de Cholesky de matrice de corrélation 
      PnlVect* sigma_; /// vecteur de volatilités
      PnlVect* spot_;  /// valeurs initiales des sous-jacents
      PnlVect* trend_;

      /**
       * Génère une trajectoire du modèle et la stocke dans path
       *
       * @param[out] path contient une trajectoire du modèle.
       * C'est une matrice de taille (nbTimeSteps+1) x d
       * @param[in] T  maturité
       * @param[in] nbTimeSteps nombre de dates de constatation
       */
      void asset(PnlMat* path, double T, int nbTimeSteps, PnlRng* rng);

      /**
       * Calcule une trajectoire du modèle connaissant le
       * passé jusqu' à la date t
       *
       * @param[out] path  contient une trajectoire du sous-jacent
       * donnée jusqu'à l'instant t par la matrice past
       * @param[in] t date jusqu'à laquelle on connait la trajectoire.
       * t n'est pas forcément une date de discrétisation
       * @param[in] nbTimeSteps nombre de pas de constatation
       * @param[in] T date jusqu'à laquelle on simule la trajectoire
       * @param[in] past trajectoire réalisée jusqu'a la date t
       */
      void asset(PnlMat* path, double t, double T, int nbTimeSteps, PnlRng* rng, const PnlMat* past);

      PnlMat* simul_market(int H, double T,  PnlRng* rng);

      /**
       * Shift d'une trajectoire du sous-jacent
       *
       * @param[in]  path contient en input la trajectoire
       * du sous-jacent
       * @param[out] shift_path contient la trajectoire path
       * dont la composante d a été shiftée par (1+h)
       * à partir de la date t.
       * @param[in] t date à partir de laquelle on shift
       * @param[in] h pas de différences finies
       * @param[in] d indice du sous-jacent à shifter
       * @param[in] timestep pas de constatation du sous-jacent
       */
      void shiftAsset(PnlMat* shift_path, const PnlMat* path, int d, double h, double t, double timestep);

      void simulate_directions(PnlVect* spots, PnlVect* normalVect, PnlVect* corr_d , double timeStep, double factorExprStep, double sqrtStep);

      void simulate_directions_market(PnlVect* spots, PnlVect* normalVect, PnlVect* corr_d , double timeStep, double sqrtStep);

  };
}