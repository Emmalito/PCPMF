#ifndef MC_PRICER_BARRIERMFOPTION_H
#define MC_PRICER_BARRIERMFOPTION_H

#include "Option.hpp"
#include <iostream>
#include <string>
#include <stdio.h>
#include <assert.h>


using namespace std;

/**
 * @brief Implémentation d'un conditional call
 * 
 */
class BarrierMFOption : public Option {
    public:
        string type;    
        double r;
        PnlVect *strikes, *dates;
        BarrierMFOption(double T_, int nbTimeSteps_, int size_, double r_, PnlVect *strikes_, PnlVect *dates_);
        double payoff(const PnlMat* path);
        virtual ~BarrierMFOption();
};
   

#endif // MC_PRICER_BARRIERMFOPTION_H
