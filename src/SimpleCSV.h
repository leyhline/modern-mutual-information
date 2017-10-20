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

#pragma once

#include <string>
#include <fstream>
#include <vector>

/**
 * Class for simple CSV parsing.
 * This reads a file of numbers and writes them into a vector.
 * The default delimiter is a single space as well as newlines.
 */
template<typename T>
	// requires Integral<T>
class SimpleCSV
{
public:
	/**
	 * Constructor.
	 * @param path Which file to parse.
	 * @param delimiter Used as separator between numbers. (default: space)
	 * 		  Note that also a newline is a hard coded delimiter.
	 */
	SimpleCSV(const std::string& path, char delimiter = ' ');

	/**
	 * Getter for resulting data parsed from file.
	 */
	std::vector<T>& getData();

private:
	const char delimiter;
	std::vector<T> data;

	void parse_file(const std::string& input);

	T stox(const std::string& str);
};
