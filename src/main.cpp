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

#include <cstdlib>
#include <iostream>
#include <fstream>
#include "SimpleCSV.h"
#include "utilities.h"

inline bool file_exists(const char* filename)
{
	std::ifstream fs(filename);
	return fs.good();
}

int main(int argc, char* argv[])
{
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

	return EXIT_SUCCESS;
}
