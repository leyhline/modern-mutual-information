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
#include <stdexcept>
#include <cstddef>

/**
 * A class for 1D-histogram calculation.
 * Usable for all integral types.
 */
template<typename T>
	// requires Integral<T>
class Histogram1d
{
public:
	/**
	 * Constructor for known range of values.
	 * If there are values outside of [min,max] they are ignored at insertion.
	 * @param bins Number of bins.
	 * @param min Minimum value in data vector.
	 * @param max Maximum value in data vector.
	 */
	Histogram1d(int bins, T min, T max);

	/**
	 * Construct class from already available histogram data.
	 * @param bins Number of bins.
	 * @param min Minimum value in data array.
	 * @param max Maximum value in data array.
	 * @param H Histogram vector holding the bins.
	 * @param count Total number of values in histogram.
	 */
	Histogram1d(int bins, T min, T max,
			std::vector<int> H, int count);

	/**
	 * Calculate the histogram single-threaded on the CPU.
	 * @param begin Iterator to the beginning of the data.
	 * @param end Iterator to the end of the data.
	 */
	template<typename Iterator>
	void calculate_cpu(const Iterator begin, const Iterator end);

	/**
	 * Given an iterator (type: int) holding index positions, this methods increments
	 * the histogram at for each position by one.
	 * If the index position is smaller than the bin number no insertion takes place.
	 * @param begin Iterator to the beginning of the index data.
	 * @param end: Iterator to the end of the index data.
	 */
	template<typename Iterator>
	void increment_cpu(const Iterator begin, const Iterator end);

	/**
	 * Get bin count as specified in constructor.
	 */
	int getBins() const;

	/**
	 * Get total number of values inserted into histogram.
	 */
	int getCount() const;

	/**
	 * Get reference to histogram vector.
	 */
	const std::vector<int>& getHistogram() const;

	/**
	 * Get (supposed) maximum value of the data.
	 */
	T getMax() const;

	/**
	 * Get (supposed) minimum value of the data.
	 */
	T getMin() const;

private:
	const int bins;
	int count;
	const T min;
	const T max;
	std::vector<int> H;

	/**
	 * Transfer function for actually doing the insertion into H.
	 * @param value Increment histogram for this value.
	 */
	void transfer(const T value);

	void check_constructor() const;
};


//////////////////
/// IMPLEMENTATION
//////////////////

template<typename T>
Histogram1d<T>::Histogram1d(int bins, T min, T max)
	: bins(bins), count(0), min(min), max(max)
{
	check_constructor();
	H.resize(bins, 0);
}

template<typename T>
Histogram1d<T>::Histogram1d(int bins, T min, T max,
	std::vector<int> H, int count)
	: bins(bins), count(count), min(min), max(max), H(H)
{
	check_constructor();
	// Casting bins to size_type because it can't be negative thanks to check_constructor method.
	if (H.size() != std::vector<int>::size_type(bins))
		throw std::invalid_argument("Argument bins has to be of same size as H vector.");
}

template<typename T>
template<typename Iterator>
void Histogram1d<T>::calculate_cpu(const Iterator begin, const Iterator end)
{
	for (auto i = begin; i != end; ++i)
		transfer(*i);
}

template<typename T>
template<typename Iterator>
void Histogram1d<T>::increment_cpu(const Iterator begin, const Iterator end)
{
	for (auto index = begin; index != end; ++index)
	{
		if (*index < bins)
		{
			++H[*index];
			++count;
		}
	}
}

template<typename T>
int Histogram1d<T>::getBins() const
{
	return bins;
}

template<typename T>
int Histogram1d<T>::getCount() const
{
	return count;
}

template<typename T>
const std::vector<int>& Histogram1d<T>::getHistogram() const
{
	return H;
}

template<typename T>
T Histogram1d<T>::getMax() const
{
	return max;
}

template<typename T>
T Histogram1d<T>::getMin() const
{
	return min;
}

template<typename T>
void Histogram1d<T>::transfer(const T value)
{
	if (value >= min && value < max)
	{
		T normalized = (value - min) / (max - min);
#pragma warning(suppress: 4244)
		int index = normalized * bins;  // Implicit conversion to integer.
		++H[index];
		++count;
	}
	else if (value == max)
	{
		++H.back();
		++count;
	}
}

template<typename T>
void Histogram1d<T>::check_constructor() const
{
	if (min >= max)
		throw std::logic_error("min has to be smaller than max.");
	if (bins < 1)
		throw std::invalid_argument("There must be at least one bin.");
}