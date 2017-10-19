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

template<typename T>
	// requires Integral<T>
class SimpleCSV
{
public:
	SimpleCSV(const std::string& path, char delimiter = ' ');

	const std::vector<T>& getData() const;

private:
	const char delimiter;
	std::vector<T> data;

	void parse_file(const std::string& input);

	T stox(const std::string& str);
};
