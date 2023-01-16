#pragma once

#include "Option.hpp"
#include <iostream>
#include <string>
#include <stdio.h>
#include <assert.h>
#include "jlparser/parser.hpp"

using namespace std;

namespace Couverture {
    class BasketOption : public Option {
        public:
            double strike_;  /// Strike
            string type_;    
            PnlVect *lambdas_, *spots_;  /// Spots créés avec S0 only
            BasketOption(double T, int nbTimeSteps, int size, PnlVect *lambdas, PnlVect *spots, double strike);
            double payoff(const PnlMat* path);
            // virtual ~BasketOption();
    };
}   
