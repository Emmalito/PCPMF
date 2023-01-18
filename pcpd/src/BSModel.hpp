#pragma once

#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "BarrierMFOption.hpp"

class BSModel
{
    public:
        int nAssets;
        double interestRate;
        PnlMat* volatility;
        BarrierMFOption* opt;

        BSModel(int nAssets, double interestRate, PnlMat* volatility, BarrierMFOption* opt);

        void asset(PnlMat* path, const PnlMat* past, double currentDate, bool isMonitoringDate, int nbTimeSteps, PnlRng* rng);
        void shiftAsset(PnlMat* shift_path, const PnlMat* path, int d, double fdStep, double currentDate, bool isMonitoringDate, double timeStep);
        void asset_ti(int i, PnlMat* simulatedMarket, double timeStep, PnlVect* normal_vect);
}
