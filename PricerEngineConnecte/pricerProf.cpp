#include <iostream>
#include "json_reader.hpp"
#include "pricer.hpp"

BlackScholesPricer::BlackScholesPricer(nlohmann::json &jsonParams) {
    jsonParams.at("VolCholeskyLines").get_to(volatility);
    jsonParams.at("MathPaymentDates").get_to(paymentDates);
    jsonParams.at("Strikes").get_to(strikes);
    jsonParams.at("DomesticInterestRate").get_to(interestRate);
    jsonParams.at("RelativeFiniteDifferenceStep").get_to(fdStep);
    jsonParams.at("SampleNb").get_to(nSamples);
    nAssets = volatility->n;
}

BlackScholesPricer::~BlackScholesPricer() {
    pnl_vect_free(&paymentDates);
    pnl_vect_free(&strikes);
    pnl_mat_free(&volatility);
}

void BlackScholesPricer::print() {
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

void BlackScholesPricer::priceAndDeltas(const PnlMat *past, double currentDate, bool isMonitoringDate, double price, double &priceStdDev, PnlVect* &deltas, PnlVect* &deltasStdDev) {
    price = 0.;
    priceStdDev = 0.;
    deltas = pnl_vect_create_from_zero(nAssets);
    deltasStdDev = pnl_vect_create_from_zero(nAssets);
    /* A compléter */
}
