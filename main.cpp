/**
 * Copyright 2017 Thomas Leyh <leyht@informatik.uni-freiburg.de>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Define some default values.
constexpr int default_shift_from {-500};
constexpr int default_shift_to   { 500};
constexpr int default_bins_x     {  10};
constexpr int default_bins_y     {  10};
constexpr int default_shift_step {   1};
constexpr int default_bootstrap_samples { 100};

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string>
#include <tclap/CmdLine.h>
#include "src/SimpleCSV.h"
#include "src/utilities.h"

inline bool file_exists(const char* filename)
{
	std::ifstream fs(filename);
	return fs.good();
}

struct float_pair {
	float first;
	float second;
};

inline float_pair find_minmax_if_nan(float min, float max,
		const std::vector<float>::iterator begin,
		const std::vector<float>::iterator end) {
	float_pair result_pair;
	if (std::isnan(min) && std::isnan(max))
	{
		auto result = std::minmax_element(begin, end);
		result_pair.first = *result.first;
		result_pair.second = *result.second;
	}
	else if (std::isnan(min))
	{
		result_pair.first = *std::min_element(begin, end);
		result_pair.second = max;
	}
	else if (std::isnan(max))
	{
		result_pair.first = min;
		result_pair.second = *std::max_element(begin, end);
	}
	else {
		result_pair.first = min;
		result_pair.second = max;
	}
	return result_pair;
}


int main(int argc, char* argv[])
{
	try
	{
		// Defining and parsing command line arguments with TCLAP (great library).
		char desc[100]; // Use this for descriptions where default value need to be appended.
		TCLAP::CmdLine cmd("Calculates mutual information by shifting over two data vectors.", ' ', "0.9");
		TCLAP::UnlabeledValueArg<std::string> path1("path1", "first data vector", true, "", "path");
		TCLAP::UnlabeledValueArg<std::string> path2("path2", "second data vector", true, "", "path");
		TCLAP::SwitchArg bootstrapping("b", "bootstrapping", "Use bootstrapping for histograms", false);
		sprintf(desc, "Number of sampled histograms for bootstrapping (default: %d)", default_bootstrap_samples);
		TCLAP::ValueArg<int> bootstrapping_samples("B", "samples", desc, false, default_bootstrap_samples, "int");
		sprintf(desc, "minimum shift of second data vector against first one; can be negative (default: %d)", default_shift_from);
		TCLAP::ValueArg<int> shift_from("f", "shift_from", desc, false, default_shift_from, "int");
		sprintf(desc, "maximum shift of second data vector against first one; can be negative (default: %d)", default_shift_to);
		TCLAP::ValueArg<int> shift_to("t", "shift_to", desc, false, default_shift_to, "int");
		sprintf(desc, "size of steps when data vectors (default: %d)", default_shift_step);
		TCLAP::ValueArg<int> shift_step("s", "shift_step", desc, false, default_shift_step, "int");
		sprintf(desc, "number of bins on x-axis of intermediate histogram (default: %d)", default_bins_x);
		TCLAP::ValueArg<int> bins_x("a", "bins_x", desc, false, default_bins_x, "int");
		sprintf(desc, "number of bins on y-axis of intermediate histogram (default: %d)", default_bins_y);
		TCLAP::ValueArg<int> bins_y("c", "bins_y", desc, false, default_bins_y, "int");
		TCLAP::ValueArg<float> min1("n", "min1", "minimum value to consider in first data vector (optional)", false, NAN, "float");
		TCLAP::ValueArg<float> max1("m", "max1", "maximum value to consider in first data vector (optional)", false, NAN, "float");
		TCLAP::ValueArg<float> min2("N", "min2", "minimum value to consider in second data vector (optional)", false, NAN, "float");
		TCLAP::ValueArg<float> max2("M", "max2", "maximum value to consider in second data vector (optional)", false, NAN, "float");
		TCLAP::ValueArg<char> delimiter("d", "delimiter", "delimiter between values in csv files (default: space)", false, ' ', "char");
		TCLAP::ValueArg<std::string> outfile("o", "outfile", "Results are written to outfile (default: stdout)", false, "", "string");
		TCLAP::ValueArg<int> input_precision("p", "in_presicion", "Precision of input file, can be 0 (CSV, default), 32 (float), 64 (double)",
										     false, 0, "int");
		cmd.add(path1);
		cmd.add(path2);
		cmd.add(delimiter);
		cmd.add(input_precision);
		cmd.add(outfile);
		cmd.add(max2);
		cmd.add(min2);
		cmd.add(max1);
		cmd.add(min1);
		cmd.add(bins_y);
		cmd.add(bins_x);
		cmd.add(shift_step);
		cmd.add(shift_to);
		cmd.add(shift_from);
		cmd.add(bootstrapping_samples);
		cmd.add(bootstrapping);
		// Parse command line arguments and do stuff accordingly.
		cmd.parse(argc, argv);
		SimpleCSV<float> input1(path1.getValue(), delimiter.getValue());
		SimpleCSV<float> input2(path2.getValue(), delimiter.getValue());
		float_pair minmax1 = find_minmax_if_nan(
				min1.getValue(), max1.getValue(), input1.getData().begin(), input1.getData().end());
		float_pair minmax2 = find_minmax_if_nan(
				min2.getValue(), max2.getValue(), input2.getData().begin(), input2.getData().end());
		std::vector<float> result;
		if (bootstrapping.getValue())
		{
			result = shifted_mutual_information_with_bootstrap(
				shift_from.getValue(), shift_to.getValue(),
				bins_x.getValue(), bins_y.getValue(),
				minmax1.first, minmax1.second,
				minmax2.first, minmax2.second,
				input1.getData().begin(), input1.getData().end(),
				input2.getData().begin(), input2.getData().end(),
				bootstrapping_samples.getValue(),
				shift_step.getValue());
		}
		else
		{
			result = shifted_mutual_information(
				shift_from.getValue(), shift_to.getValue(),
				bins_x.getValue(), bins_y.getValue(),
				minmax1.first, minmax1.second,
				minmax2.first, minmax2.second,
				input1.getData().begin(), input1.getData().end(),
				input2.getData().begin(), input2.getData().end(),
				shift_step.getValue());
		}
		if (outfile.getValue().empty())
		{
			std::cout << result[0];
			for (std::size_t i = 1, max = result.size(); i < max; ++i)
			{
				std::cout << delimiter.getValue() << result[i];
			}
			std::cout << std::endl;
		}
		else
		{
			SimpleCSV<float> outputFile(outfile.getValue(), delimiter.getValue());
			outputFile.writeData(result);
		}
		
	}
	catch (TCLAP::ArgException &e)
	{
		std::cerr << "error: " << e.error() << " for argument " << e.argId() << std::endl;
		return EXIT_FAILURE;
	}
	catch (std::logic_error &e)
	{
		std::cerr << "error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

