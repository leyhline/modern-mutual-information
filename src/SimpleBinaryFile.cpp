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

#include <cctype>
#include <stdexcept>
#include "SimpleBinaryFile.h"

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
        while(fs.good())
		{
			if ( fs.read((char*)&out, sizeof(Prec)) )
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

// Compile for these instances:
//		For float:
template class SimpleBinaryFile<float>;
template class SimpleBinaryFile<double>;