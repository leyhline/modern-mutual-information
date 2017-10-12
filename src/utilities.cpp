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
#include <iterator>
#include <climits>

#include "utilities.h"
#include "Histogram2d.h"

template<typename T, typename Iterator>
std::vector<unsigned int> calculate_indices_1d(
		const unsigned int bins,
		const T min, const T max,
		const Iterator begin, const Iterator end)
{
	if (min >= max)
		throw std::logic_error("min has to be smaller than max.");
	if (bins < 1)
		throw std::invalid_argument("There must be at least one bin.");
	size_t size = std::distance(begin, end);
	std::vector<unsigned int> result(size);
	// Most code token from Histogram1d class.
	for (size_t i = 0; i < size; ++i)
	{
		auto value = begin[i];
		if (value >= min && value < max)
		{
			T normalized = (value - min) / (max - min);
			unsigned int index = normalized * bins;  // Implicit conversion to integer.
			result[i] = index;
		}
		else if (value == max)
		{
			result[i] = bins - 1;
		}
		else
		{
			result[i] = UINT_MAX;
		}
	}
	return result;
}

template<typename T, typename Iterator>
std::vector<index_pair> calculate_indices_2d(
		const unsigned int binsX, const unsigned int binsY,
		const T minX, const T maxX,
		const T minY, const T maxY,
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
	std::vector<index_pair> result(sizeX);
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
			result[i] = index_pair {indexX, indexY}; // I hope this is allowed.
		}
		else
		{
			result[i] = index_pair {UINT_MAX, UINT_MAX};
		}
	}
	return result;
}

template<typename T>
inline void check_shifted_mutual_information(
		const size_t sizeX, const size_t sizeY,
		const int shift_from, const int shift_to,
		const unsigned int binsX, const unsigned int binsY,
		const T minX, const T maxX, const T minY, const T maxY, const int shift_step)
{
	if (sizeX != sizeY)
		throw std::logic_error("Containers referenced by iterators must have the same size.");
	if (shift_from >= shift_to)
		throw std::logic_error("shift_from has to be smaller than shift_to.");
	if (minX >= maxX)
		throw std::logic_error("minX has to be smaller than maxX.");
	if (minY >= maxY)
		throw std::logic_error("minY has to be smaller than maxY.");
	if (binsX < 1)
		throw std::invalid_argument("There must be at least one binX.");
	if (binsY < 1)
		throw std::invalid_argument("There must be at least one binY.");
	if ((size_t)(shift_to < 0 ? -shift_to : shift_to) >= sizeX)
		throw std::logic_error("Maximum shift does not fit data size.");
	if ((size_t)(shift_from < 0 ? -shift_from : shift_from) >= sizeX)
		throw std::logic_error("Minimum shift does not fit data size.");
	if (shift_step < 1)
		throw std::invalid_argument("shift_step must be greater or equal 1.");
}

template<typename T, typename Iterator>
std::vector<T> shifted_mutual_information(
		const int shift_from, const int shift_to,
		const unsigned int binsX, const unsigned int binsY,
		const T minX, const T maxX, const T minY, const T maxY,
		const Iterator beginX, const Iterator endX,
		const Iterator beginY, const Iterator endY,
		const int shift_step /* 1 */)
{
	size_t sizeX = std::distance(beginX, endX);
	size_t sizeY = std::distance(beginY, endY);
	check_shifted_mutual_information(sizeX, sizeY, shift_from, shift_to,
								     binsX, binsY, minX, maxX, minY, maxY, shift_step);
	std::vector<unsigned int> indicesX = calculate_indices_1d(binsX, minX, maxX, beginX, endX);
	std::vector<unsigned int> indicesY = calculate_indices_1d(binsY, minY, maxY, beginY, endY);
	std::vector<T> result((shift_to - shift_from) / shift_step);
	#pragma omp parallel for
	for (int i = shift_from; i < shift_to; i += shift_step)
	{
		Histogram2d<T> hist(binsX, binsY, minX, maxX, minY, maxY);
		if (i < 0)
		{
			hist.increment_cpu(indicesX.begin(), std::prev(indicesX.end(), -i),
					           std::next(indicesY.begin(), -i), indicesY.end());
		}
		else
		{
			hist.increment_cpu(std::next(indicesX.begin(), i), indicesX.end(),
							   indicesY.begin(), std::prev(indicesY.end(), i));
		}
		result[(i - shift_from) / shift_step] = *hist.calculate_mutual_information();
	}
	return result;
}

// Compile for these instances.
typedef std::vector<float>::iterator fvec_iter;
template std::vector<unsigned int> calculate_indices_1d(unsigned int, float, float, fvec_iter, fvec_iter);
template std::vector<index_pair> calculate_indices_2d(
		unsigned int, unsigned int, float, float, float, float, fvec_iter, fvec_iter, fvec_iter, fvec_iter);
template std::vector<float> shifted_mutual_information(int, int, unsigned int, unsigned int,
													   float, float, float, float,
													   fvec_iter, fvec_iter, fvec_iter, fvec_iter, int);
