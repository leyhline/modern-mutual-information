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
	Histogram1d(unsigned int bins, T min, T max);

	/**
	 * Construct class from already available histogram data.
	 * @param bins Number of bins.
	 * @param min Minimum value in data array.
	 * @param max Maximum value in data array.
	 * @param H Histogram vector holding the bins.
	 * @param count Total number of values in histogram.
	 */
	Histogram1d(unsigned int bins, T min, T max,
			std::vector<unsigned int> H, unsigned int count);

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
	unsigned int getBins() const;

	/**
	 * Get total number of values inserted into histogram.
	 */
	unsigned int getCount() const;

	/**
	 * Get reference to histogram vector.
	 */
	const std::vector<unsigned int>& getHistogram() const;

	/**
	 * Get (supposed) maximum value of the data.
	 */
	T getMax() const;

	/**
	 * Get (supposed) minimum value of the data.
	 */
	T getMin() const;

private:
	const unsigned int bins;
	unsigned int count;
	const T min;
	const T max;
	std::vector<unsigned int> H;

	/**
	 * Transfer function for actually doing the insertion into H.
	 * @param value Increment histogram for this value.
	 */
	void transfer(const T value);

	void check_constructor() const;
};
