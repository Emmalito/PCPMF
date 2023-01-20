#include "pricer.hpp"
#include "pnl/pnl_finance.h"

using namespace std;

int main(int argc, char **argv)
{
    /**
     * @brief Test du calcul du prix monte carlo de l'option BarrierMFOption (CondionalCall)
     * 
     * On se base sur les données de math_param_1.json.
     * On demande veut le prix en t = 0.4, qui se trouve avant la 3e payment date.
     */

    PnlMat *volatility = pnl_mat_create(4, 4);
    pnl_mat_set(volatility, 0, 0, 0.3);
    pnl_mat_set(volatility, 1, 0, 0.024);
    pnl_mat_set(volatility, 1, 1, 0.29903845906505067);
    pnl_mat_set(volatility, 2, 0, 0.024);
    pnl_mat_set(volatility, 2, 1, 0.02215099696778153);
    pnl_mat_set(volatility, 2, 2, 0.29821692328460053);
    pnl_mat_set(volatility, 3, 0, 0.024);
    pnl_mat_set(volatility, 3, 1, 0.02215099696778153);
    pnl_mat_set(volatility, 3, 2, 0.02056668436445521);
    pnl_mat_set(volatility, 3, 3, 0.29750688198357056);

    PnlVect *paymentDates = pnl_vect_create(4);
    pnl_vect_set(paymentDates, 0, 0.1626984126984127);
    pnl_vect_set(paymentDates, 1, 0.25396825396825395);
    pnl_vect_set(paymentDates, 2, 0.5198412698412699);
    pnl_vect_set(paymentDates, 3, 0.8531746031746031);

    PnlVect *strikes = pnl_vect_create(4);
    pnl_vect_set(strikes, 0, 10);
    pnl_vect_set(strikes, 1, 10.1);
    pnl_vect_set(strikes, 2, 10.2);
    pnl_vect_set(strikes, 3, 10.3);

    double interestRate = 0.01, fdStep = 0.1;
    int nSamples = 10000;

    PnlMat* past = pnl_mat_create(3, 4);
    pnl_mat_set(past, 0, 0, 8.0);
    pnl_mat_set(past, 0, 1, 8.5);
    pnl_mat_set(past, 0, 2, 9.0);
    pnl_mat_set(past, 0, 3, 10.0);
    pnl_mat_set(past, 1, 0, 8.5);
    pnl_mat_set(past, 1, 1, 8.0);
    pnl_mat_set(past, 1, 2, 9.78);
    pnl_mat_set(past, 1, 3, 10.3);
    pnl_mat_set(past, 2, 0, 9.4);
    pnl_mat_set(past, 2, 1, 8.3);
    pnl_mat_set(past, 2, 2, 7.5);
    pnl_mat_set(past, 2, 3, 9.3);

    double currentDate = 0.4;
    bool isMonitoringDate = false;
    double price, priceStdDev;
    PnlVect *deltas, *deltaStdDev;

    BlackScholesPricer pricer(volatility, paymentDates, strikes, interestRate, fdStep, nSamples);
    pricer.priceAndDeltas(past, currentDate, isMonitoringDate, price, priceStdDev, deltas, deltaStdDev);

    cout << "Prix(" << currentDate << ") = " << price << endl;
    cout << "PrixStdDev(" << currentDate << ") = " << priceStdDev << endl;
    cout << "Deltas(" << currentDate << ") :" << endl;
    pnl_vect_print(deltas);
    cout << "DeltasStdDev(" << currentDate << ") :" << endl;
    pnl_vect_print(deltaStdDev);

    pnl_mat_free(&volatility);
    pnl_mat_free(&past);    
    pnl_vect_free(&paymentDates);
    pnl_vect_free(&strikes);
    pnl_vect_free(&deltas);
    pnl_vect_free(&deltaStdDev);
    return 0;
}
