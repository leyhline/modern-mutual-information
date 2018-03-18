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

enum Precision
{
	PREC_32 = 32, // float
	PREC_64 = 64  // double
};

/**
 * A very simple class for reading and writing from binary files.
 */
template<typename T>
	// requires Integral<T>
class SimpleBinaryFile : public ISimpleFile<T>
{
public:
	/**
	 * Constructor.
	 * @param path Which file to parse.
     * @param Precision of processed file (32 or 64 allowed)
	 */
	SimpleBinaryFile(const std::string& path, Precision precision);

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
    Precision precision;
	std::vector<T> data;

    template<typename Prec>
	void parse_file(const std::string& input);
};


//////////////////
/// IMPLEMENTATION
//////////////////

template<typename T>
SimpleBinaryFile<T>::SimpleBinaryFile(const std::string& path, Precision precision)
	: path(path), precision(precision)
{
}

template<typename T>
std::vector<T>& SimpleBinaryFile<T>::getData()
{
	if (data.size() == 0)
	{
		if (precision == PREC_32)
			parse_file<float>(path);
		else if (precision == PREC_64)
			parse_file<double>(path);
		else
			throw std::invalid_argument("Precision must PREC_32 or PREC_64.");
	}
	return data;
}

template<typename T>
void SimpleBinaryFile<T>::writeData(const std::vector<T>& data_to_write)
{
	std::ofstream fs(path, std::ofstream::binary);
	if (fs.is_open())
	{
		fs.write((char*)data_to_write.data(),
			data_to_write.size() * sizeof(T));
	}
	else
	{
		std::string what_arg("Could not open file: ");
		what_arg.append(path);
		throw std::runtime_error(what_arg);
	}
	fs.close();
}

template<typename T>
template<typename Prec>
void SimpleBinaryFile<T>::parse_file(const std::string& path) {
	std::ifstream fs(path, std::ifstream::binary);
	if (fs.is_open())
	{
		Prec out;
		while (fs.good())
		{
			if (fs.read((char*)&out, sizeof(Prec)))
				data.push_back(out);
		}
	}
	else
	{
		std::string what_arg("Could not open file: ");
		what_arg.append(path);
		throw std::runtime_error(what_arg);
	}
	fs.close();
}