#pragma once

#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"

/// \brief Modèle de Black Scholes
class BlackScholesModel
{

  public:
    int size_;       /// nombre d'actifs du modèle
    double r_;       /// taux d'intérêt
    double rho_;     /// paramètre de corrélation
    PnlVect* sigma_; /// vecteur de volatilités
    PnlVect* spot_;  /// valeurs initiales des sous-jacents
    PnlVect* trend_;  /// tendance du modèle
    PnlVect* gaussian_simulation_;  /// vecteurs gaussien actualisé pour chaque calcul
    PnlMat* L_; /// Matrice de Cholesky

    /**
     * Constructeur du modele de BlackScholes avec L donné
     */
    BlackScholesModel(int size, double r, PnlMat* L, PnlVect* sigma, PnlVect* spot);


    /**
     * Constructeur du modele de BlackScholes
     */
    BlackScholesModel(int size, double r, double rho, PnlVect* sigma, PnlVect* spot);

    /**
     * Constructeur du modele de BlackScholes avec trend
     */
    BlackScholesModel(int size, double r, double rho, PnlVect* sigma, PnlVect* spot, PnlVect* trend);

    /**
     * Destructeur du modele
     */
     ~BlackScholesModel();

    /**
     * Fonction de création de la matrice de L_ Choleski grace à la matrice Gamma,
     * Gamma = 1 sur la diagonale et rho partout ailleurs.
     * Gamma = L_L_'
     */
    PnlMat* compute_cholesky();

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
     * Fonction utilitaire du calcul du prix dans asset pour compute une trajectoire au temps i
     * Le traitement diffère selon si ti est une date de constatation ou non.
     * @param[in] offset est un entier qui calibre la ligne à laquelle on écrit.
     * Si on se trouve à constatation, offset=1 et 0 sinon
     * @param[in] i ligne computée
     */
    void asset_at_time_i(int i, PnlMat* path, double pas, PnlRng* rng, int offset = 0);

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
     * @param[in] set_subblock boolean permettant d'optimiser le code en evitant de reset la matrice
     */
    void shiftAsset(PnlMat* shift_path, const PnlMat* path, int d, double h, double t, double timestep, bool set_subblock=true);

    /**
     * Génèse d'une simulation du marché suivant le modèle de BlackScholes
     *
     * @param[out] simulatedMarket contient une trajectoire simulé sous la probabilité historique du modèle.
     * C'est une matrice de taille (nbTimeSteps+1) x d
     * @param[in] H  nombre de dates H, typiquement on prendra une date par jour
     */
    void simul_market(PnlMat* simulatedMarket, double H, double T, PnlRng* rng);
    void simulate_at_time_i(int i, PnlMat* path, double pas, PnlRng* rng);
};
