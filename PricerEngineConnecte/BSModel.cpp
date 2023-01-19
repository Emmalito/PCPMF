#include <iostream>
#include "BSModel.hpp"

using namespace std;

BSModel::BSModel(int nAssets_, double interestRate_, PnlMat* volatility_, PnlVect* discretisationDates_)
{
    nAssets = nAssets_;
    interestRate = interestRate_;
    volatility = pnl_mat_copy(volatility_);
    discretisationDates = pnl_vect_copy(discretisationDates_);
}

BSModel::~BSModel()
{
    pnl_mat_free(&volatility);
}

void BSModel::asset(PnlMat* path, const PnlMat* past, double currentDate, bool isMonitoringDate, int nbTimeSteps, double T, PnlRng* rng)
{
    pnl_mat_set_subblock(path, past, 0, 0);
    if (currentDate < T)
    {
        PnlVect* normal_vect = pnl_vect_create(nAssets);
        pnl_vect_rng_normal(normal_vect, nAssets, rng);

        int k = 0;
        double timeStep;
        while (currentDate > pnl_vect_get(discretisationDates, k))
        {
            k++;
            if (k >= discretisationDates->size) break;
        }
        
        if (!isMonitoringDate)
        {
            timeStep = pnl_vect_get(discretisationDates, k) - currentDate;
            asset_ti(past->m-1, path, timeStep, normal_vect, 1);
        }
        
        for (int i = past->m; i <  nbTimeSteps; i++){
            if (discretisationDates->size != 1)
                timeStep = pnl_vect_get(discretisationDates, i) - pnl_vect_get(discretisationDates, i-1);
            else
                timeStep = pnl_vect_get(discretisationDates, i-1) - currentDate;
            pnl_vect_rng_normal(normal_vect, nAssets, rng);
            asset_ti(i, path, timeStep, normal_vect, 0);
        }
        pnl_vect_free(&normal_vect);
    }
}

void BSModel::asset_ti(int i, PnlMat* path, double timeStep, PnlVect* normal_vect, int offset)
{
    double rand_part, determ_part, res, sigma, sqrtTimeStep = sqrt(timeStep);
    for(int d = 0; d < nAssets; d++){
        PnlVect vol_d = pnl_vect_wrap_mat_row(volatility, d);
        sigma = pnl_vect_norm_two(&vol_d);
        determ_part = (interestRate - (sigma * sigma)/2) * timeStep;
        rand_part = sqrtTimeStep * pnl_vect_scalar_prod(&vol_d, normal_vect);
        res = pnl_mat_get(path, i-1 + offset, d) * exp(determ_part + rand_part);
        pnl_mat_set(path, i, d, res);
    }
}

void BSModel::shiftAsset(PnlMat* shift_path, const PnlMat* path, int d, double fdStep, double currentDate, bool isMonitoringDate)
{
    pnl_mat_set_subblock(shift_path, path, 0, 0);
    int k = 0;
    while (currentDate > pnl_vect_get(discretisationDates, k))
    {
        k++;
        if (k >= discretisationDates->size) break;
    }

    int start_index;
    if (k == 0) start_index = 0;
    else if (isMonitoringDate) start_index = k - 1;
    else start_index = k;

    for (int i = start_index; i < path->m; i++)
    {
        pnl_mat_set(shift_path, i, d, pnl_mat_get(path, i, d) * (1 + fdStep));
    }
}
