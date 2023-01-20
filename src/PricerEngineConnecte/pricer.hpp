#pragma once

#include <iostream>
#include <nlohmann/json.hpp>
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "pnl/pnl_random.h"
#undef OK
#include "BarrierMFOption.hpp"
#include "BSModel.hpp"


class BlackScholesPricer {
public:
    PnlMat *volatility;
    PnlVect *paymentDates;
    PnlVect *strikes;
    int nAssets;
    double interestRate;
    double fdStep;
    int nSamples;
    double T;
    BarrierMFOption* opt;
    BSModel* model;
    PnlRng* rng;

    BlackScholesPricer(nlohmann::json &jsonParams);
    
    // Constructeur pour les fichiers test_pricer*
    BlackScholesPricer(
        PnlMat* volatility_,
        PnlVect* paymentDates_,
        PnlVect* strikes_,
        double interestRate_,
        double fdStep_,
        int nSamples_
    );

    ~BlackScholesPricer();
    void priceAndDeltas(
        const PnlMat *past,
        double currentDate,
        bool isMonitoringDate,
        double &price,
        double &priceStdDev,
        PnlVect* &deltas,
        PnlVect* &deltasStdDev
    );

    void print();
};
