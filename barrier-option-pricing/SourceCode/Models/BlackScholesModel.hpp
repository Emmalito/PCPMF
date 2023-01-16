#pragma once

#include <vector>
#include "UnderlyingModel.hpp"
#include "BlackScholesModelInputParser.hpp"
#include "BlackScholesParametersHelper.hpp"

namespace models
{

	class BlackScholesModel : public UnderlyingModel
	{
	public:
		BlackScholesModel(const input_parsers::BlackScholesModelInputParser &parser, const generators::RandomGeneration &random_generator);
		double final_simulation_date() const { return final_simulation_date_; }
		double timestep() const { return timestep_; }
		int total_historical_path_size() const { return generated_asset_paths_->m; }
		const PnlMat* const simulate_asset_paths_unsafe(const double from_time, const PnlMat * const past_values) const;
		const PnlMat* const simulate_asset_paths_from_start(const PnlVect * const spot) const;
		~BlackScholesModel();


	private:

		double final_simulation_date_;
		double timestep_;

		const generators::RandomGeneration &random_generator_;

		PnlMat * generated_asset_paths_;
		PnlVect * gaussian_vector_for_simulation_;

		BlackScholesParametersHelper underlying_parameters_helper;

		void add_one_simulation_to_generated_asset_paths_unsafe(int at_line, double timelength, const PnlMat * const past_values) const;
		void fill_remainder_of_generated_asset_paths(int from_line) const;
	};

}