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

#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <cctype>
#include <stdexcept>

#include "ISimpleFile.h"

/**
 * Class for simple CSV parsing.
 * This reads a file of numbers and writes them into a vector.
 * The default delimiter is a single space as well as newlines.
 */
template<typename T>
	// requires Integral<T>
class SimpleCSV : public ISimpleFile<T>
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
	std::vector<T>& getData() override;

	/**
	 * Write data to file.
	 */
	void writeData(const std::vector<T>& data_to_write) override;

private:
	const std::string path;
    const char delimiter;
	std::vector<T> data;

	void parse_file(const std::string& input);

	T stox(const std::string& str);
};


//////////////////
/// IMPLEMENTATION
//////////////////

template<typename T>
SimpleCSV<T>::SimpleCSV(const std::string& path, char delimiter /* ' ' */)
	: path(path), delimiter(delimiter)
{
}

template<typename T>
std::vector<T>& SimpleCSV<T>::getData()
{
	if (data.size() == 0)
		parse_file(this->path);
	return data;
}

template<typename T>
void SimpleCSV<T>::writeData(const std::vector<T>& data_to_write)
{
	std::ofstream fs(this->path);
	if (fs.is_open())
	{
		fs << data_to_write[0];
		for (std::size_t i = 1, max = data_to_write.size(); i < max; ++i)
		{
			fs << this->delimiter << data_to_write[i];
		}
	}
	else
	{
		std::string what_arg("Could not open file: ");
		what_arg.append(this->path);
		throw std::runtime_error(what_arg);
	}
	fs.close();
}

template<typename T>
void SimpleCSV<T>::parse_file(const std::string& path) {
	std::ifstream fs(this->path);
	if (fs.is_open())
	{
		std::string number;
		while (fs.good())
		{
			// Read file per char.
			char c = fs.get();
			if (c == this->delimiter || c == '\n')
			{
				if (!number.empty())
				{
					data.push_back(stox(number));
					number.clear();
				}
			}
			else if (!isspace(c))
			{
				number.push_back(c);
			}
		}
	}
	else
	{
		std::string what_arg("Could not open file: ");
		what_arg.append(this->path);
		throw std::runtime_error(what_arg);
	}
	fs.close();
}

// Template specialization for conversion to float or double.
template<typename T>
T SimpleCSV<T>::stox(const std::string& str) {}

template<>
float SimpleCSV<float>::stox(const std::string& str)
{
	return std::stof(str);
}

template<>
double SimpleCSV<double>::stox(const std::string& str)
{
	return std::stod(str);
}