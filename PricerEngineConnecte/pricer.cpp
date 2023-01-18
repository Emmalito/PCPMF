#include <iostream>
#include "json_reader.hpp"
#include "pricer.hpp"
#include <ctime>

// tata = new list<c++>
// toto: grpc --> toto.add(tata)

BlackScholesPricer::BlackScholesPricer(nlohmann::json &jsonParams)
{
    jsonParams.at("VolCholeskyLines").get_to(volatility);
    jsonParams.at("MathPaymentDates").get_to(paymentDates);
    jsonParams.at("Strikes").get_to(strikes);
    jsonParams.at("DomesticInterestRate").get_to(interestRate);
    jsonParams.at("RelativeFiniteDifferenceStep").get_to(fdStep);
    jsonParams.at("SampleNb").get_to(nSamples);
    nAssets = volatility->n;
    T = pnl_vect_get(paymentDates, nAssets-1);
    BarrierMFOption opt(T, nAssets, nAssets, interestRate, strikes, paymentDates);
    BSModel mod(nAssets, interestRate, volatility, &opt);
    model = &mod;
    PnlRng* rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));
}

BlackScholesPricer::~BlackScholesPricer()
{
    pnl_vect_free(&paymentDates);
    pnl_vect_free(&strikes);
    pnl_mat_free(&volatility);
}

void BlackScholesPricer::print()
{
    std::cout << "nAssets: " << nAssets << std::endl;
    std::cout << "fdStep: " << fdStep << std::endl;
    std::cout << "nSamples: " << nSamples << std::endl;
    std::cout << "strikes: ";
    pnl_vect_print_asrow(strikes);
    std::cout << "paymentDates: ";
    pnl_vect_print_asrow(paymentDates);
    std::cout << "volatility: ";
    pnl_mat_print(volatility);
}

void BlackScholesPricer::priceAndDeltas(const PnlMat *past, double currentDate, bool isMonitoringDate, double price, double &priceStdDev, PnlVect* &deltas, PnlVect* &deltasStdDev)
{
    price = 0.;
    priceStdDev = 0.;
    deltas = pnl_vect_create_from_zero(nAssets);
    deltasStdDev = pnl_vect_create_from_zero(nAssets);
    
    double esp = 0, esp2 = 0;
    double timestep = opt->T/opt->nbTimeSteps;
    double const esperanceConstante = exp(-model->interestRate * (opt->T - currentDate)) / (2 * fdStep * nSamples);
    double const varianceConstante = esperanceConstante * esperanceConstante;

    PnlMat* path = pnl_mat_create(opt->nbTimeSteps + 1, nAssets);
    PnlMat* shiftPath = pnl_mat_create(opt->nbTimeSteps + 1, nAssets);

    double delta_d, payoff, payoff_sh_plus, payoff_sh_minus;

    for(int j=0; j<nSamples; j++)
    {
        model->asset(path, past, currentDate, isMonitoringDate, opt->nbTimeSteps, rng);
        payoff = opt->payoff(path);
        esp += payoff;
        esp2 += payoff * payoff;
        for (int d = 0; d < nAssets; d++)
        {
            model->shiftAsset(shiftPath, path, d, fdStep, currentDate, isMonitoringDate, timestep);
            payoff_sh_plus = opt->payoff(shiftPath);
            model->shiftAsset(shiftPath, path, d, -fdStep, currentDate, isMonitoringDate, timestep);
            payoff_sh_minus = opt->payoff(shiftPath);
            delta_d = payoff_sh_plus - payoff_sh_minus;
            pnl_vect_set(deltas, pnl_vect_get(deltas, d) + delta_d, d);
            pnl_vect_set(deltasStdDev, pnl_vect_get(deltasStdDev, d) + delta_d * delta_d, d);
        }
    }
    esp /= nAssets;
    esp2 /= nAssets;
    price = exp(-model->interestRate * (opt->T - currentDate)) * esp;
    priceStdDev = sqrt((exp(-2 * model->interestRate * (opt->T - currentDate)) * esp2 - price * price) / nAssets);

    double fact;
    for (int d = 0; d < opt->size; d++){
        delta_d = pnl_vect_get(deltas, d);
        pnl_vect_set(deltas, delta_d * esperanceConstante / pnl_mat_get(past, past->m - 1, d), d);
        fact = varianceConstante * nAssets / (pnl_mat_get(past, past->m - 1, d) * pnl_mat_get(past, past->m - 1, d));
        pnl_vect_set(deltasStdDev, sqrt((pnl_vect_get(deltasStdDev, d) * fact, d - delta_d * delta_d) / nAssets), d);
    }
}