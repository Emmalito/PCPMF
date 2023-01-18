#include "pricer.hpp"
#include "pnl/pnl_finance.h"

using namespace std;

int main(int argc, char **argv)
{
    PnlMat *volatility = pnl_mat_create(1, 1);
    PnlVect *paymentDates = pnl_vect_create(1), *strikes = pnl_vect_create(1);
    double interestRate = 0.01, fdStep = 0.1;
    int nSamples = 10000;
    pnl_mat_set_all(volatility, 0.3);
    pnl_vect_set(paymentDates, 0, 1.0);
    pnl_vect_set(strikes, 0, 10.);

    double pp, pd;
    int ii = pnl_cf_call_bs (8.0, 10., 1.0, interestRate, 0.0, 0.3, &pp, &pd);
    // int ii = pnl_bs_call(8.0, 10., 1.0, interestRate, 0.0, 0.3);

    PnlMat* past = pnl_mat_create(1, 1);
    pnl_mat_set_all(past, 8.0);
    double currentDate = 0.0;
    bool isMonitoringDate = true;
    double price, priceStdDev;
    PnlVect *deltas, *deltaStdDev;

    BlackScholesPricer pricer(volatility, paymentDates, strikes, interestRate, fdStep, nSamples);
    // pricer.print();
    pricer.priceAndDeltas(past, currentDate, isMonitoringDate, price, priceStdDev, deltas, deltaStdDev);

    cout << "Prix en " << currentDate << " = " << price << endl;
    pnl_vect_print(deltas);

    cout << "pp = " << pp << endl;
    cout << "pd = " << pd << endl;

    return 0;
}
