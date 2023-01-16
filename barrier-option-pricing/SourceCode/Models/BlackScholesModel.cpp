#include <cmath>
#include "BlackScholesModel.hpp"
#include "GlobalConstants.hpp"
#include "CustomExceptions.hpp"
#include <iostream>

using namespace input_parsers;
using namespace generators;
using namespace infrastructure;
using namespace models;

BlackScholesModel::BlackScholesModel(const BlackScholesModelInputParser &parser, const RandomGeneration &random_generator) :
random_generator_(random_generator)
{
	underlying_number_ = parser.get_underlying_number();
	interest_rate_ = parser.get_interest_rate();
	final_simulation_date_ = parser.get_final_simulation_date();
	int monitoring_times = parser.get_monitoring_times();
	timestep_ = static_cast<double>(final_simulation_date_) / monitoring_times;
	generated_asset_paths_ = pnl_mat_create(monitoring_times+1, underlying_number_);
	gaussian_vector_for_simulation_ = pnl_vect_create(underlying_number_);

	double correlation_parameter = parser.get_correlation_parameter();
	PnlVect * volatilities = parser.get_volatility();
	underlying_parameters_helper = BlackScholesParametersHelper(underlying_number_, interest_rate_, timestep_, correlation_parameter, volatilities);
}



const PnlMat* const BlackScholesModel::simulate_asset_paths_unsafe(const double from_time, const PnlMat * const past_values) const
{
	//TODO: extra unit and functional tests needed to make sure this implementation is correct
	pnl_mat_set_subblock(generated_asset_paths_, past_values, 0, 0);
	int number_of_values = past_values->m;
	double timespan_to_monitoring = (number_of_values - 1) * timestep_ - from_time;
	bool is_at_monitoring_time = timespan_to_monitoring < TIME_PRECISION;	
	if (!is_at_monitoring_time)
	{
		add_one_simulation_to_generated_asset_paths_unsafe(number_of_values-1, timespan_to_monitoring, past_values);
	}
	fill_remainder_of_generated_asset_paths(number_of_values);
	return generated_asset_paths_;
}


const PnlMat* const BlackScholesModel::simulate_asset_paths_from_start(const PnlVect * const spot) const
{
	pnl_mat_set_row(generated_asset_paths_, spot, 0);
	fill_remainder_of_generated_asset_paths(1);
	return generated_asset_paths_;
}


void BlackScholesModel::add_one_simulation_to_generated_asset_paths_unsafe(int at_line, double timelength, const PnlMat * const past_values) const
{
	random_generator_.get_one_gaussian_sample(gaussian_vector_for_simulation_);
	double sqrt_timelength = sqrt(timelength);
	for (int i = 0; i < underlying_number_; i++)
	{
		double current_spot = MGET(past_values, at_line, i);
		double new_underlying_value = underlying_parameters_helper.compute_one_asset_simulation(i, current_spot, interest_rate_, sqrt_timelength, gaussian_vector_for_simulation_);					
		MLET(generated_asset_paths_, at_line, i) = new_underlying_value;
	}
}


void BlackScholesModel::fill_remainder_of_generated_asset_paths(int from_line) const
{
	int last_line = generated_asset_paths_->m;
	for (int line = from_line; line < last_line; line++)
	{
		random_generator_.get_one_gaussian_sample(gaussian_vector_for_simulation_);
		for (int i = 0; i < underlying_number_; i++)
		{
			double current_spot = MGET(generated_asset_paths_, line - 1, i);
			double new_underlying_value = underlying_parameters_helper.compute_one_asset_simulation_with_precomputed_parameters(i, current_spot, gaussian_vector_for_simulation_);
			MLET(generated_asset_paths_, line, i) = new_underlying_value;
		}
	}
}


BlackScholesModel::~BlackScholesModel()
{
	pnl_mat_free(&generated_asset_paths_);
	pnl_vect_free(&gaussian_vector_for_simulation_);
	generated_asset_paths_ = nullptr;
	gaussian_vector_for_simulation_ = nullptr;
}
