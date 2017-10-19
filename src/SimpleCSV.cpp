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
#include "SimpleCSV.h"

template<typename T>
SimpleCSV<T>::SimpleCSV(const std::string& path, char delimiter /* ' ' */)
	: delimiter(delimiter)
{
	parse_file(path);
}

template<typename T>
const std::vector<T>& SimpleCSV<T>::getData() const {
	return data;
}

template<typename T>
void SimpleCSV<T>::parse_file(const std::string& path) {
	std::ifstream fs(path);
	if (fs.is_open())
	{
		std::string number;
		while(fs.good())
		{
			// Read file per char.
			char c = fs.get();
			if (c == delimiter)
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
		throw std::runtime_error("File could not be opened.");
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

// Compile for these instances:
//		For float:
template class SimpleCSV<float>;
