#include <iostream>
#include <string>
#include "jlparser/parser.hpp"

using namespace std;

int
main(int argc, char** argv)
{
    double T, r, strike;
    PnlVect *spot, *sigma, *divid;
    string type;
    int size;
    size_t n_samples;

    char* infile = argv[1];
    Param* P = new Parser(infile);

    P->extract("option type", type);
    P->extract("maturity", T);
    P->extract("option size", size);
    P->extract("spot", spot, size);
    P->extract("volatility", sigma, size);
    P->extract("interest rate", r);
    if (P->extract("dividend rate", divid, size, true) == false) {
        divid = pnl_vect_create_from_zero(size);
    }
    P->extract("strike", strike);
    P->extract("sample number", n_samples);

    P->print();
    cout << endl;
    cout << "option type " << type << endl;
    cout << "maturity " << T << endl;
    cout << "strike " << strike << endl;
    cout << "option size " << size << endl;
    cout << "interest rate " << r << endl;
    cout << "dividend rate ";
    pnl_vect_print_asrow(divid);
    cout << "spot ";
    pnl_vect_print_asrow(spot);
    cout << "volatility ";
    pnl_vect_print_asrow(sigma);
    cout << "Number of samples " << n_samples << endl;

    pnl_vect_free(&spot);
    pnl_vect_free(&sigma);
    pnl_vect_free(&divid);
    delete P;

    exit(0);
}
