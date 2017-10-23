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

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string>
#include <tclap/CmdLine.h>
#include "SimpleCSV.h"
#include "utilities.h"

inline bool file_exists(const char* filename)
{
	std::ifstream fs(filename);
	return fs.good();
}

int main(int argc, char* argv[])
{
	try
	{
		// Defining and parsing command line arguments with TCLAP (great library).
		TCLAP::CmdLine cmd("Calculates mutual information by shifting over two data vectors.", ' ', "0.9");
		TCLAP::UnlabeledValueArg<std::string> path1("path1", "first data vector", true, "", "path");
		cmd.add(path1);
		TCLAP::UnlabeledValueArg<std::string> path2("path2", "second data vector", true, "", "path");
		cmd.add(path2);
		TCLAP::ValueArg<int> shift_from("f", "shift_from",
				"minimum shift of second data vector against first one (can be negative)", false, default_shift_from, "int");
		cmd.add(shift_from);
		TCLAP::ValueArg<int> shift_to("t", "shift_to",
				"maximum shift of second data vector against first one (can be negative)", false, default_shift_to, "int");
		cmd.add(shift_to);
		TCLAP::ValueArg<int> bins_x("b", "bins_x",
				"number of bins on x-axis of intermediate histogram", false, default_bins_x, "int");
		cmd.add(bins_x);
		TCLAP::ValueArg<int> bins_y("c", "bins_y",
				"number of bins on y-axis of intermediate histogram", false, default_bins_y, "int");
		cmd.add(bins_y);
		cmd.parse(argc, argv);
		const char delimiter = ' ';
		SimpleCSV<float> input1(path1.getValue(), delimiter);
		SimpleCSV<float> input2(path2.getValue(), delimiter);
		float minX = 1;
		float maxX = 3;
		float minY = 1;
		float maxY = 3;
		int shift_step = 1;
		auto result = shifted_mutual_information(
			shift_from.getValue(), shift_to.getValue(),
			bins_x.getValue(), bins_y.getValue(),
			minX, maxX, minY, maxY,
			input1.getData().begin(), input1.getData().end(),
			input2.getData().begin(), input2.getData().end(),
			shift_step);
		std::cout << result[0];
		for (float v : result)
		{
			std::cout << delimiter << v;
		}
		std::cout << std::endl;
	}
	catch (TCLAP::ArgException &e)
	{
		std::cerr << "error: " << e.error() << " for argument " << e.argId() << std::endl;
	}
	catch (std::runtime_error &e)
	{
		std::cerr << "error: " << e.what() << std::endl;
	}
	return EXIT_SUCCESS;
	/*
	if (argc != 3)
	{
		std::cout << "Usage: " << argv[0]
				  << " FILE1 FILE2" << std::endl
				  << "       "
				  << "FILE1 and FILE2 are holding numeric values in CSV format, delimited by space."
				  << std::endl;
		return EXIT_FAILURE;
	}

	if (!file_exists(argv[1]))
	{
		std::cout << "Could not open file: " << argv[1] << std::endl;
		return EXIT_FAILURE;
	}
	if (!file_exists(argv[2]))
	{
		std::cout << "Could not open file: " << argv[2] << std::endl;
		return EXIT_FAILURE;
	}

	const char delimiter = ' ';

	SimpleCSV<float> input1(argv[1], delimiter);
	SimpleCSV<float> input2(argv[2], delimiter);

	int shift_from = -2;
	int shift_to = 2;
	int binsX = 10;
	int binsY = 10;
	float minX = 1;
	float maxX = 3;
	float minY = 1;
	float maxY = 3;
	int shift_step = 1;

	auto result = shifted_mutual_information(
			shift_from, shift_to, binsX, binsY, minX, maxX, minY, maxY,
			input1.getData().begin(), input1.getData().end(),
			input2.getData().begin(), input2.getData().end(),
			shift_step);

	std::cout << result[0];
	for (float v : result)
	{
		std::cout << delimiter << v;
	}
	std::cout << std::endl;
	*/

	return EXIT_SUCCESS;
}
