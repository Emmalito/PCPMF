//
// Created by lionel on 12/09/22.
//

#ifndef MC_PRICER_ASIANOPTION_H
#define MC_PRICER_ASIANOPTION_H
#include "Option.hpp"

class AsianOption : public Option
{
  private:
    double k_;   /// Strike
    PnlVect* weights_; /// Poids des sous jacents de l'option; weights->size == path->n == _size


  public:
    AsianOption(double T, int nbTimeSteps, int size, double k, PnlVect * weights);
    ~AsianOption();
    double payoff(const PnlMat* path);
};

#endif // MC_PRICER_ASIANOPTION_H
