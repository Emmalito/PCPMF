#include <iostream>
#include "BSModel.hpp"

using namespace std;

BSModel::BSModel(int nAssets_, double interestRate_, PnlMat* volatility_)
{
    nAssets = nAssets_;
    interestRate = interestRate_;
    volatility = volatility_;
}
/*
BSModel::~BSModel() {
    pnl_mat_free(&volatility);
}*/

void BSModel::asset2(PnlMat* path, const PnlMat* past, double currentDate, bool isMonitoringDate, int nbTimeSteps, double T, PnlRng* rng)
{
    pnl_mat_set_subblock(path, past, 0, 0);
    if (currentDate < T)
    {
        int k = (int) (currentDate * (double) nbTimeSteps / T);
        double t_i_1 = (k + 1) * T/nbTimeSteps;
        
        int start_index = past->m - 1;
        int offset = 1;
        if (isMonitoringDate)
        {
            start_index++;
            offset = 0;
        }

        PnlVect* normal_vect = pnl_vect_create(nAssets);
        pnl_vect_rng_normal(normal_vect, nAssets, rng);

        double timeStep = t_i_1 - currentDate;
        asset_ti(start_index, path, timeStep, normal_vect, offset);

        timeStep = T/nbTimeSteps;
        for (int i = start_index + 1; i <= nbTimeSteps; i++){
            pnl_vect_rng_normal(normal_vect, nAssets, rng);
            asset_ti(i, path, timeStep, normal_vect, offset);
        }
    }
}

void BSModel::asset(PnlMat* path, const PnlMat* past, double currentDate, bool isMonitoringDate, int nbTimeSteps, double T, PnlRng* rng)
{
    pnl_mat_set_subblock(path, past, 0, 0);
    if (currentDate < T)
    {
        PnlVect* normal_vect = pnl_vect_create(nAssets);
        pnl_vect_rng_normal(normal_vect, nAssets, rng);

        if (!isMonitoringDate)
        {
            int k = (int) (currentDate * (double) nbTimeSteps / T);
            double t_i = (double) (k + 1) * T / (double) nbTimeSteps;
            asset_ti(past->m-1, path, t_i - currentDate, normal_vect, 1);
        }
        double timeStep = T / (double) nbTimeSteps;
        for (int i = past->m; i <= nbTimeSteps; i++){
            pnl_vect_rng_normal(normal_vect, nAssets, rng);
            asset_ti(i, path, timeStep, normal_vect, 0);
        }
    }
}

void BSModel::asset_ti(int i, PnlMat* simulatedMarket, double timeStep, PnlVect* normal_vect, int offset)
{
    double rand_part, determ_part, res, sigma, sqrtTimeStep = sqrt(timeStep);
    for(int d = 0; d < nAssets; d++){
        PnlVect vol_d = pnl_vect_wrap_mat_row(volatility, d);
        sigma = pnl_vect_norm_two(&vol_d);
        determ_part = (interestRate - (sigma * sigma)/2) * timeStep;
        rand_part = sqrtTimeStep * pnl_vect_scalar_prod(&vol_d, normal_vect);
        res = pnl_mat_get(simulatedMarket, i-1 + offset, d) * exp(determ_part + rand_part);
        pnl_mat_set(simulatedMarket, i, d, res);
    }
}

void BSModel::shiftAsset(PnlMat* shift_path, const PnlMat* path, int d, double fdStep, double currentDate, bool isMonitoringDate, double timeStep)
{
    double k = (int) (currentDate / (double) timeStep);

    int start_index;
    if (isMonitoringDate) start_index = k;
    else start_index = k + 1;

    for (int i = start_index; i < path->m; i++)
    {
        pnl_mat_set(shift_path, i, d, pnl_mat_get(path, i, d) * (1 + fdStep));
    }
}
