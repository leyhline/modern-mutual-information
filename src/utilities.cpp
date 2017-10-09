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

#include <stdexcept>

#include "utilities.h"

template<typename T, typename Iterator>
std::vector<unsigned int> calculate_indices_1d(
		unsigned int bins,
		T min, T max,
		const Iterator begin, const Iterator end)
{
	if (min >= max)
		throw std::logic_error("min has to be smaller than max.");
	if (bins < 1)
		throw std::invalid_argument("There must be at least one bin.");
	size_t size = std::distance(begin, end);
	std::vector<unsigned int> result;
	// Most code token from Histogram1d class.
	for (size_t i = 0; i < size; ++i)
	{
		auto value = begin[i];
		if (value >= min && value < max)
		{
			T normalized = (value - min) / (max - min);
			unsigned int index = normalized * bins;  // Implicit conversion to integer.
			result.push_back(index);
		}
		else if (value == max)
		{
			result.push_back(bins - 1);
		}
	}
	return result;
}

template<typename T, typename Iterator>
std::vector<index_pair> calculate_indices_2d(
		unsigned int binsX, unsigned int binsY,
		T minX, T maxX,
		T minY, T maxY,
		const Iterator beginX, const Iterator endX,
		const Iterator beginY, const Iterator endY)
{
	if (minX >= maxX)
		throw std::logic_error("minX has to be smaller than maxX.");
	if (minY >= maxY)
		throw std::logic_error("minY has to be smaller than maxY.");
	if (binsX < 1)
		throw std::invalid_argument("There must be at least one binX.");
	if (binsY < 1)
		throw std::invalid_argument("There must be at least one binY.");
	size_t sizeX = std::distance(beginX, endX);
	size_t sizeY = std::distance(beginY, endY);
	if (sizeX != sizeY)
		throw std::logic_error("Containers referenced by iterators must have the same size.");
	std::vector<index_pair> result;
	// Most code token from Histogram2d class.
	for (size_t i = 0; i < sizeX; ++i)
	{
		auto x = beginX[i];
		auto y = beginY[i];
		if (   x >= minX
			&& x <= maxX
			&& y >= minY
			&& y <= maxY)
		{
			unsigned int indexX;
			if (x == maxX)
				indexX = binsX - 1;
			else
				indexX = (x - minX) / (maxX - minX) * binsX;
			unsigned int indexY;
			if (y == maxY)
				indexY = binsY - 1;
			else
				indexY = (y - minY) / (maxY - minY) * binsY;
			result.push_back(index_pair {indexX, indexY} ); // I hope this is allowed.
		}
	}
	return result;
}

template<typename T, typename Iterator>
std::vector<T> shifted_mutual_information(
		int shift_from, int shift_to,
		const Iterator beginX, const Iterator endX,
		const Iterator beginY, const Iterator endY)
{
}

// Compile for these instances.
typedef std::vector<float>::iterator fvec_iter;
template std::vector<unsigned int> calculate_indices_1d(unsigned int, float, float, fvec_iter, fvec_iter);
template std::vector<index_pair> calculate_indices_2d(
		unsigned int, unsigned int, float, float, float, float, fvec_iter, fvec_iter, fvec_iter, fvec_iter);
