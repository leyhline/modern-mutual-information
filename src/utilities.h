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

#include <vector>
#include <iterator>
#include <cstddef>

template<typename T, typename Iterator>
std::vector<unsigned int> calculate_indices_1d(
		unsigned int bins,
		T min, T max,
		const Iterator begin, const Iterator end);

struct index_pair
{
	unsigned int first;
	unsigned int second;
};

template<typename T, typename Iterator>
std::vector<index_pair> calculate_indices_2d(
		unsigned int binsX, unsigned int binsY,
		T minX, T maxX,
		T minY, T maxY,
		const Iterator beginX, const Iterator endX,
		const Iterator beginY, const Iterator endY);

template<typename T, typename Iterator>
std::vector<T> shifted_mutual_information(
		int shift_from, int shift_to,
		const Iterator beginX, const Iterator endX,
		const Iterator beginY,
		const Iterator endY);
