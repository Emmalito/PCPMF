//
// Created by lionel on 12/09/22.
//

#ifndef MC_PRICER_PERFORMANCEOPTION_H
#define MC_PRICER_PERFORMANCEOPTION_H

#include "Option.hpp"
class PerformanceOption : public Option
{
  private:
    PnlVect* weights_; /// Poids des sous jacents de l'option
  public:
    PerformanceOption(double T, int nbTimeSteps, int size, PnlVect * weights);
    ~PerformanceOption();
    double payoff(const PnlMat* path);
};

#endif // MC_PRICER_PERFORMANCEOPTION_H
