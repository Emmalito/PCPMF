#pragma once

#include "pnl/pnl_random.h"
#undef OK
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"

class BSModel
{
    public:
        int nAssets;
        double interestRate;
        PnlMat* volatility;
        PnlVect* discretisationDates;

        BSModel(int nAssets_, double interestRate_, PnlMat* volatility_, PnlVect* discretisationDates_);
        ~BSModel();
        void asset(PnlMat* path, const PnlMat* past, double currentDate, bool isMonitoringDate, int nbTimeSteps, double T, PnlRng* rng);
        void shiftAsset(PnlMat* shift_path, const PnlMat* path, int d, double fdStep, double currentDate, bool isMonitoringDate);
        void asset_ti(int i, PnlMat* path, double timeStep, PnlVect* normal_vect, int offset);
};
