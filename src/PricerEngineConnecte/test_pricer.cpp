#include "pricer.hpp"
#include "pnl/pnl_finance.h"

using namespace std;

int main(int argc, char **argv)
{
    /**
     * @brief Test du calcul du prix monte carlo contre le prix théorique 
     * de l'option BarrierMFOption (CondionalCall)
     * 
     * On se base sur un seul actif S1 avec une unique payment Date en T = 1, et on veut le prix en 0.
     * Cette option devient équivalente à une option vanille.
     */

    PnlMat *volatility = pnl_mat_create(1, 1);
    PnlVect *paymentDates = pnl_vect_create(1), *strikes = pnl_vect_create(1);
    double interestRate = 0.01, fdStep = 0.1;
    int nSamples = 10000;
    pnl_mat_set_all(volatility, 0.3);
    pnl_vect_set(paymentDates, 0, 1.0);
    pnl_vect_set(strikes, 0, 10.);

    PnlMat* past = pnl_mat_create(1, 1);
    pnl_mat_set_all(past, 8.0);
    double currentDate = 0.0;
    bool isMonitoringDate = false;
    double price, priceStdDev;
    PnlVect *deltas, *deltaStdDev;
    BlackScholesPricer pricer(volatility, paymentDates, strikes, interestRate, fdStep, nSamples);
    

    // Prix attendu d'une option vanille
    double expectedPrice, expectedDeltas;
    int i = pnl_cf_call_bs (8.0, 10., 1.0, interestRate, 0.0, 0.3, &expectedPrice, &expectedDeltas);

    // Prix monte-carlo de l'option vanille
    pricer.priceAndDeltas(past, currentDate, isMonitoringDate, price, priceStdDev, deltas, deltaStdDev);

    cout << "Prix MonteCarlo(" << currentDate << ") = " << price << endl;
    cout << "Deltas MonteCarlo(" << currentDate << ") :" << endl;
    pnl_vect_print(deltas);


    cout << endl;
    cout << "expectedPrice(" << currentDate << ") = " << expectedPrice << endl;
    cout << "expectedDeltas(" << currentDate << ") :" << endl;
    cout << expectedDeltas << endl;

    pnl_mat_free(&volatility);
    pnl_mat_free(&past);    
    pnl_vect_free(&paymentDates);
    pnl_vect_free(&strikes);
    pnl_vect_free(&deltas);
    pnl_vect_free(&deltaStdDev);
    return 0;
}
