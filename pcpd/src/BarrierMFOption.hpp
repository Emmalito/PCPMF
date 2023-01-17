#ifndef MC_PRICER_BARRIERMFOPTION_H
#define MC_PRICER_BARRIERMFOPTION_H

#include "Option.hpp"
#include <iostream>
#include <string>
#include <stdio.h>
#include <assert.h>
#include "jlparser/parser.hpp"


using namespace std;

class BarrierMFOption : public Option {
    public:
        string type;    
        double r;
        PnlVect *strikes, *dates;
        BarrierMFOption(double T, int nbTimeSteps, int size, double r, PnlVect *strikes, PnlVect *dates);
        double payoff(const PnlMat* path);
        virtual ~BarrierMFOption();
};
   

#endif // MC_PRICER_BARRIERMFOPTION_H
