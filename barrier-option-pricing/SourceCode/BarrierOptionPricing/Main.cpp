#include <iostream>
#include <ctime>
#include "MonteCarloPricer.hpp"
//#include "Parser.hpp"
#include "CoreBarrierOptionInputParser.hpp"
#include "CoreBlackScholesModelInputParser.hpp"
#include "PnlRandomGeneration.hpp"
#include "BlackScholesModel.hpp"
#include "BarrierOptionParameters.hpp"

using namespace std;
using namespace input_parsers;
using namespace models;
using namespace options;
using namespace generators;

int main(int argc, char **argv)
{	
	const char * input_file = argv[1];
	Parser core_parser(input_file);
	CoreBarrierOptionInputParser option_parser(core_parser);
	CoreBlackScholesModelInputParser model_parser(core_parser);
	PnlRandomGeneration pnl_generator;
	BlackScholesModel bs_model(model_parser, pnl_generator);
	BarrierOptionParameters option_parameters(option_parser);
	BarrierOption tested_option(option_parameters);
	int number_samples;
	core_parser.extract("sample number", number_samples);
	MonteCarloPricer mc_pricer(number_samples);
	int nb_underlyings = tested_option.underlying_number();
	PnlVect * spots;
	core_parser.extract("spot", spots, nb_underlyings);
	double price = 0;
	double confidence_interval = 0;

	clock_t start_time = clock();
	mc_pricer.price(bs_model, tested_option, spots, price, confidence_interval);
	clock_t end_time = clock();
	cout << "price: " << price << " confidence interval: " << confidence_interval << endl;
	cout << "computed in " << (double)(end_time - start_time) / CLOCKS_PER_SEC << " seconds" << endl;
}