/**
* Copyright 2018, University of Freiburg
* Optophysiology Lab.
* Thomas Leyh <thomas.leyh@mailbox.org>
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
constexpr int default_bootstrap_samples {100};
constexpr int default_bootstrap_reps    {100};

#include <memory>
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
#include "src/SimpleBinaryFile.h"
#include "src/utilities.h"

template<typename T, typename Iterator>
inline T calc_mean(Iterator begin, Iterator end)
{
	T mean = 0;
	size_t size = std::distance(begin, end);
	for (size_t i = 0; i < size; ++i)
	{
		mean += begin[i];
	}
	return mean / T(size);
}

template<typename T, typename Iterator>
inline T calc_std(Iterator begin, Iterator end, T mean)
{
	T std = 0;
	size_t size = std::distance(begin, end);
	for (size_t i = 0; i < size; ++i)
	{
		T temp = begin[i] - mean;
		std += temp * temp;
	}
	return std::sqrt(std / T(size));
}

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
		sprintf(desc, "Repeat bootstrapping R times for mean and std. derivation (default: %d)", default_bootstrap_reps);
		TCLAP::ValueArg<int> bootstrapping_reps("R", "repetitions", desc, false, default_bootstrap_reps, "int");
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
		TCLAP::ValueArg<std::string> outfile("o", "outfile", "Results are written to outfile.bin or outfile.csv (default: stdout)", false, "", "string");
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
		cmd.add(bootstrapping_reps);
		cmd.add(bootstrapping_samples);
		cmd.add(bootstrapping);
		// Parse command line arguments and do stuff accordingly.
		cmd.parse(argc, argv);
		std::unique_ptr<ISimpleFile<float>> input1;
		std::unique_ptr<ISimpleFile<float>> input2;
		int precision = input_precision.getValue();
		char delim = delimiter.getValue();
		if (precision == 0)
		{
			input1 = std::unique_ptr<ISimpleFile<float>>(new SimpleCSV<float>(path1.getValue(), delim));
			input2 = std::unique_ptr<ISimpleFile<float>>(new SimpleCSV<float>(path2.getValue(), delim));
		}
		else
		{
			Precision prec = static_cast<Precision>(precision);
			input1 = std::unique_ptr<ISimpleFile<float>>(new SimpleBinaryFile<float>(path1.getValue(), prec));
			input2 = std::unique_ptr<ISimpleFile<float>>(new SimpleBinaryFile<float>(path2.getValue(), prec));
		}
		float_pair minmax1 = find_minmax_if_nan(
				min1.getValue(), max1.getValue(), input1->getData().begin(), input1->getData().end());
		float_pair minmax2 = find_minmax_if_nan(
				min2.getValue(), max2.getValue(), input2->getData().begin(), input2->getData().end());
		std::vector<float> result;
		if (bootstrapping.getValue())
		{
			// These are `nr_repetition` vectors of mutual information
			auto whole_result = shifted_mutual_information_with_bootstrap(
				shift_from.getValue(), shift_to.getValue(),
				bins_x.getValue(), bins_y.getValue(),
				minmax1.first, minmax1.second,
				minmax2.first, minmax2.second,
				input1->getData().begin(), input1->getData().end(),
				input2->getData().begin(), input2->getData().end(),
				bootstrapping_samples.getValue(),
				bootstrapping_reps.getValue(), shift_step.getValue());
			result.resize(whole_result.size() * 2);
			// Now take the mean over all repetitions:
			for (int i = 0, end = whole_result.size(); i < end; ++i)
			{
				result[i] = calc_mean<float>(whole_result[i].begin(), whole_result[i].end());
			}
			for (int i = 0, end = whole_result.size(); i < end; ++i)
			{
				result[end + i] = calc_std<float>(whole_result[i].begin(), whole_result[i].end(), result[i]);
			}
		}
		else
		{
			result = shifted_mutual_information(
				shift_from.getValue(), shift_to.getValue(),
				bins_x.getValue(), bins_y.getValue(),
				minmax1.first, minmax1.second,
				minmax2.first, minmax2.second,
				input1->getData().begin(), input1->getData().end(),
				input2->getData().begin(), input2->getData().end(),
				shift_step.getValue());
		}
		std::string outfile_path = outfile.getValue();
		if (outfile_path.empty())
		{
			std::cout << result[0];
			for (std::size_t i = 1, max = result.size(); i < max; ++i)
			{
				std::cout << delim << result[i];
			}
			std::cout << std::endl;
		}
		else
		{
			if (outfile_path.size() > 4 && outfile_path.substr(outfile_path.size() - 4) == ".csv")
			{
				SimpleCSV<float> outputFile(outfile_path, delim);
				outputFile.writeData(result);
			} else
			{
				SimpleBinaryFile<float> outputFile(outfile_path, PREC_32);
				outputFile.writeData(result);
			}
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
