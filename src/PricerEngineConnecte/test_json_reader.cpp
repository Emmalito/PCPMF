#include <iostream>
#include <fstream>
#include "json_reader.hpp"


int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Wrong number of arguments. Exactly one argument is required" << std::endl;
        std::exit(0);
    }
    std::ifstream ifs(argv[1]);
    nlohmann::json j = nlohmann::json::parse(ifs);
    PnlMat *volatility;
    PnlVect *paymentDates;
    j.at("VolCholeskyLines").get_to(volatility);
    j.at("MathPaymentDates").get_to(paymentDates);
    std::cout << "Volatility matrix" << std::endl;
    pnl_mat_print(volatility);
    std::cout << "Payment dates ";
    pnl_vect_print_asrow(paymentDates);
    pnl_mat_free(&volatility);
    pnl_vect_free(&paymentDates);
    exit(0);
}