/**
* Copyright 2018, University of Freiburg
* Optophysiology Lab.
* Thomas Leyh <leyh.thomas@web.de>
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
#include <cstddef>
#include <random>
#include <stdexcept>
#include <iterator>
#include <climits>
#include <chrono>

#include "Histogram2d.h"

/**
 * Calculates the histogram indices of a certain data container.
 * @param bins Number of bins for the imaginary histogram.
 * @param min Minimum value in data container.
 * @param max Maximum value in data container.
 * @param begin Iterator to the beginning of the data.
 * @param end Iterator to the end of the data.
 * @return A vector with the same size as the data holding the index positions.
 * 	       If there is a value outside the [min,max] range UINT_MAX is inserted
 * 	       at this position.
 */
template<typename T, typename Iterator>
std::vector<int> calculate_indices_1d(
		const int bins,
		const T min, const T max,
		const Iterator begin, const Iterator end);

/**
 * Small struct for simply holding two index values.
 */
struct index_pair
{
	int first;
	int second;
};

/**
 * Calculate indices of a 2d histogram for a certain data container.
 * @param binsX Number of bins on imaginary histogram's x-axis.
 * @param binsY Number of bins on imaginary histogram's y-axis.
 * @param minX Minimum value in first data container.
 * @param maxX Maximum value in first data container.
 * @param minY Minimum value in second data container.
 * @param maxY Maximum value in second data container.
 * @param beginX Iterator to the beginning of the first data container.
 * @param endX Iterator to the end of the first data container.
 * @param beginY Iterator to the beginning of the second data container.
 * @param endY Iterator to the end of the second data container.
 * 			   Both containers must have the same size.
 * @return Vector of index_pair structs. The vector has the same size as the data.
 *         If there is a value outside the [min,max] range index_pair {UINT_MAX, UINT_MAX}
 *         is inserted at this position.
 */
template<typename T, typename Iterator>
std::vector<index_pair> calculate_indices_2d(
		const int binsX, const int binsY,
		const T minX, const T maxX,
		const T minY, const T maxY,
		const Iterator beginX, const Iterator endX,
		const Iterator beginY, const Iterator endY);

/**
 * Calculate a certain range of mutual information by shifting the second data container
 * in relation to the first one.
 * No shift:
 * 		|--------------------| dataX
 * 		|--------------------| dataY
 * Negative shift:
 * 		|--------------------| dataX
 * 	|--------------------| 	   dataY
 * Positive shift:
 * 		|--------------------| 	   dataX
 * 			|--------------------| dataY
 * @param shift_from Negative (or positive) value for specifying the largest shift to the left.
 * @param shift_to Positive (of negative) value for specifying the largest shift to the right.
 * 	      Has to be greater than shift_from.
 * @param minX Minimum value in first data container.
 * @param maxX Maximum value in first data container.
 * @param minY Minimum value in second data container.
 * @param maxY Maximum value in second data container.
 * @param beginX Iterator to the beginning of the first data container.
 * @param endX Iterator to the end of the first data container.
 * @param beginY Iterator to the beginning of the second data container.
 * @param endY Iterator to the end of the second data container.
 * 			   Both containers must have the same size.
 * @param shift_step (Optional) Specifies the steps between shifts. Default = 1.
 * @return Vector with size (shift_to - shift_from) holding the mutual information for each shift.
 * 		   Might be smaller if shift_step is specified.
 */
template<typename T, typename Iterator>
std::vector<T> shifted_mutual_information(
		const int shift_from, const int shift_to,
		const int binsX, const int binsY,
		const T minX, const T maxX, const T minY, const T maxY,
		const Iterator beginX, const Iterator endX,
		const Iterator beginY, const Iterator endY,
		const int shift_step = 1);

/**
 * Calculates the mutual information of the two given data vectors X and Y
 * by using bootstrapping. This is done by first generating nr_samples
 * histograms by sampling the data and then again sampling these histograms
 * and adding them together.
 * Helper function for shifted_mutual_information_with_bootstrap.
 */
template<typename T, typename Iterator>
T bootstrapped_mi(const Iterator beginX, const Iterator endX,
				  const Iterator beginY, const Iterator endY,
				  const int binsX, const int binsY,
				  const T minX, const T maxX, const T minY, const T maxY,
				  int nr_samples, std::mt19937& rgen);

/**
 * Similar to shifted_mutual_information but additionally uses bootstrapping
 * this increasing its runtime. There are two additional parameters:
 * @param nr_samples How many temporary histograms to generate by sampling the data.
 * @param nr_repetitions How many times to repeat this process to minimize noise.
 * @return A vector of size `(shift_to - shift_from) / shift_step + 1`
	holding vectors of size `nr_repetitions` with the mutual information.
 */
template<typename T, typename Iterator>
std::vector< std::vector<T> > shifted_mutual_information_with_bootstrap(
		const int shift_from, const int shift_to,
		const int binsX, const int binsY,
		const T minX, const T maxX, const T minY, const T maxY,
		const Iterator beginX, const Iterator endX,
		const Iterator beginY, const Iterator endY,
		int nr_samples, int nr_repetitions,
		const int shift_step = 1);

/**
 * This is for the matlab mex interface:
 * Instead of returning a vector the result is written to a pointer location.
 * The values specified by beginX, endX, beginY, endY are the histogram indices in range [0, nr_bins).
 * @param output A pointer to to a vector of size (shift_to - shift_from) / shift_step + 1
 */
template<typename T>
void shifted_mutual_information(
	    const int shift_from, const int shift_to,
		const int binsX, const int binsY,
		const T minX, const T maxX, const T minY, const T maxY,
		const int* beginX, const int* endX,
		const int* beginY, const int* endY,
		const int shift_step,
		T* output);

/**
 * This is for the matlab mex interface:
 * Instead of returning a vector the result is written to a pointer location.
 * The values specified by beginX, endX, beginY, endY are the histogram indices in range [0, nr_bins).
 * @param output A pointer to to a vector of size ((shift_to - shift_from) / shift_step + 1) * nr_repetitions
 */
template<typename T>
void shifted_mutual_information_with_bootstrap(
		const int shift_from, const int shift_to,
		const int binsX, const int binsY,
		const T minX, const T maxX, const T minY, const T maxY,
		const int* beginX, const int* endX,
		const int* beginY, const int* endY,
		int nr_samples, int nr_repetitions,
		const int shift_step,
		T* output);

//////////////////
/// IMPLEMENTATION
//////////////////

template<typename T, typename Iterator>
std::vector<int> calculate_indices_1d(
	const int bins,
	const T min, const T max,
	const Iterator begin, const Iterator end)
{
	if (min >= max)
		throw std::logic_error("min has to be smaller than max.");
	if (bins < 1)
		throw std::invalid_argument("There must be at least one bin.");
	int size = std::distance(begin, end);
	std::vector<int> result(size);
	// Most code token from Histogram1d class.
#pragma omp parallel for
	for (int i = 0; i < size; ++i)
	{
		auto value = begin[i];
		if (value >= min && value < max)
		{
			T normalized = (value - min) / (max - min);
#pragma warning(suppress: 4244)
			int index = normalized * bins;  // Implicit conversion to integer.
			result[i] = index;
		}
		else if (value == max)
		{
			result[i] = bins - 1;
		}
		else
		{
			result[i] = INT_MAX;
		}
	}
	return result;
}

template<typename T, typename Iterator>
std::vector<index_pair> calculate_indices_2d(
	const int binsX, const int binsY,
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
	int sizeX = std::distance(beginX, endX);
	int sizeY = std::distance(beginY, endY);
	if (sizeX != sizeY)
		throw std::logic_error("Containers referenced by iterators must have the same size.");
	std::vector<index_pair> result(sizeX);
	// Most code token from Histogram2d class.
#pragma omp parallel for
	for (int i = 0; i < sizeX; ++i)
	{
		auto x = beginX[i];
		auto y = beginY[i];
		if (x >= minX
			&& x <= maxX
			&& y >= minY
			&& y <= maxY)
		{
			int indexX;
			if (x == maxX)
				indexX = binsX - 1;
			else
#pragma warning(suppress: 4244)
				indexX = (x - minX) / (maxX - minX) * binsX;
			int indexY;
			if (y == maxY)
				indexY = binsY - 1;
			else
#pragma warning(suppress: 4244)
				indexY = (y - minY) / (maxY - minY) * binsY;
			result[i] = index_pair{ indexX, indexY }; // I hope this is allowed.
		}
		else
		{
			result[i] = index_pair{ INT_MAX, INT_MAX };
		}
	}
	return result;
}

template<typename T>
inline void check_shifted_mutual_information(
	const size_t sizeX, const size_t sizeY,
	const int shift_from, const int shift_to,
	const int binsX, const int binsY,
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
	const int binsX, const int binsY,
	const T minX, const T maxX, const T minY, const T maxY,
	const Iterator beginX, const Iterator endX,
	const Iterator beginY, const Iterator endY,
	const int shift_step /* 1 */)
{
	size_t sizeX = std::distance(beginX, endX);
	size_t sizeY = std::distance(beginY, endY);
	check_shifted_mutual_information(sizeX, sizeY, shift_from, shift_to,
		binsX, binsY, minX, maxX, minY, maxY, shift_step);
	std::vector<int> indicesX = calculate_indices_1d(binsX, minX, maxX, beginX, endX);
	std::vector<int> indicesY = calculate_indices_1d(binsY, minY, maxY, beginY, endY);
	auto indX_begin = indicesX.begin();
	auto indX_end = indicesX.end();
	auto indY_begin = indicesY.begin();
	auto indY_end = indicesY.end();
	std::vector<T> result((shift_to - shift_from) / shift_step + 1);
#pragma omp parallel for
	for (int i = shift_from; i <= shift_to; i += shift_step)
	{
		Histogram2d<T> hist(binsX, binsY, minX, maxX, minY, maxY);
		if (i < 0)
		{
			hist.increment_cpu(indX_begin, std::prev(indX_end, -i),
				std::next(indY_begin, -i), indY_end);
		}
		else if (i > 0)
		{
			hist.increment_cpu(std::next(indX_begin, i), indX_end,
				indY_begin, std::prev(indY_end, i));
		}
		else // Should not be necessary but better be explicit.
		{
			hist.increment_cpu(indX_begin, indX_end,
				indY_begin, indY_end);
		}
		result[(i - shift_from) / shift_step] = *hist.calculate_mutual_information();
	}
	return result;
}

template<typename T, typename Iterator>
T bootstrapped_mi(const Iterator beginX, const Iterator endX,
	const Iterator beginY, const Iterator endY,
	const int binsX, const int binsY,
	const T minX, const T maxX, const T minY, const T maxY,
	int nr_samples, std::mt19937& rgen)
{
	size_t sizeX = std::distance(beginX, endX);
	size_t sizeY = std::distance(beginY, endY);
	if (sizeX != sizeY)
		throw std::logic_error("Containers referenced by iterators must have the same size.");
	std::uniform_int_distribution<int> uniform(0, sizeX - 1);
	std::vector<Histogram2d<T>*> hist3d(nr_samples); // A vector of raw pointers :(
	int nr_samples_per_histogram = sizeX / nr_samples;
	// First create some histograms from randomly sampled data pairs.
	for (int sample = 0; sample < nr_samples; ++sample)
	{
		Histogram2d<T>* hist_ptr = new Histogram2d<T>(binsX, binsY, minX, maxX, minY, maxY);
		for (int i = 0; i < nr_samples_per_histogram; ++i)
		{
			int ridx = uniform(rgen);
			hist_ptr->increment_at(beginX[ridx], beginY[ridx]);
		}
		hist3d[sample] = hist_ptr;
	}
	// Now sample these histograms again and add them together.
	std::uniform_int_distribution<int> uniform_from_samples(0, nr_samples - 1);
	Histogram2d<T> final_hist(binsX, binsY, minX, maxX, minY, maxY);
	for (int sample = 0; sample < nr_samples; ++sample)
	{
		int sampleidx = uniform_from_samples(rgen);
		final_hist.add(*hist3d[sampleidx]);
	}
	// Cleanup (even though using raw pointers is not really elegant)
	for (int sample = 0; sample < nr_samples; ++sample)
	{
		delete hist3d[sample];
	}
	return *final_hist.calculate_mutual_information();
}

template<typename T, typename Iterator>
std::vector< std::vector<T> > shifted_mutual_information_with_bootstrap(
	const int shift_from, const int shift_to,
	const int binsX, const int binsY,
	const T minX, const T maxX, const T minY, const T maxY,
	const Iterator beginX, const Iterator endX,
	const Iterator beginY, const Iterator endY,
	int nr_samples, int nr_repetitions,
	const int shift_step /* 1 */)
{
	size_t sizeX = std::distance(beginX, endX);
	size_t sizeY = std::distance(beginY, endY);
	check_shifted_mutual_information(sizeX, sizeY, shift_from, shift_to,
		binsX, binsY, minX, maxX, minY, maxY, shift_step);
	if (nr_samples < 1)
		throw std::logic_error("For bootstrapping you need a minimum of one sample.");
	if (nr_repetitions < 1)
		throw std::logic_error("There needs to be at least one repetition of the bootstrapping process.");
	std::vector<int> indicesX = calculate_indices_1d(binsX, minX, maxX, beginX, endX);
	std::vector<int> indicesY = calculate_indices_1d(binsY, minY, maxY, beginY, endY);
	auto indX_begin = indicesX.begin();
	auto indX_end = indicesX.end();
	auto indY_begin = indicesY.begin();
	auto indY_end = indicesY.end();
	std::vector< std::vector<T> > result((shift_to - shift_from) / shift_step + 1,
										 std::vector<T>(nr_repetitions));
#pragma omp parallel for
	for (int i = shift_from; i <= shift_to; i += shift_step)
	{
		unsigned int seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
		std::mt19937 rgen(seed);
		for (int j = 0; j < nr_repetitions; ++j)
		{
			T mi;
			if (i < 0)
			{
				mi = bootstrapped_mi<T>(indX_begin, std::prev(indX_end, -i),
					std::next(indY_begin, -i), indY_end,
					binsX, binsY, minX, maxX, minY, maxY, nr_samples, rgen);
			}
			else if (i > 0)
			{
				mi = bootstrapped_mi<T>(std::next(indX_begin, i), indX_end,
					indY_begin, std::prev(indY_end, i),
					binsX, binsY, minX, maxX, minY, maxY, nr_samples, rgen);
			}
			else // Should not be necessary but better be explicit.
			{
				mi = bootstrapped_mi<T>(indX_begin, indX_end,
					indY_begin, indY_end,
					binsX, binsY, minX, maxX, minY, maxY, nr_samples, rgen);
			}
			result[(i - shift_from) / shift_step][j] = mi;
		}
	}
	return result;
}

template<typename T>
void shifted_mutual_information(
	const int shift_from, const int shift_to,
	const int binsX, const int binsY,
	const T minX, const T maxX, const T minY, const T maxY,
	const int* beginX, const int* endX,
	const int* beginY, const int* endY,
	const int shift_step,
	T* output)
{
	size_t sizeX = std::distance(beginX, endX);
	size_t sizeY = std::distance(beginY, endY);
	check_shifted_mutual_information(sizeX, sizeY, shift_from, shift_to,
		binsX, binsY, minX, maxX, minY, maxY, shift_step);
#pragma omp parallel for
	for (int i = shift_from; i <= shift_to; i += shift_step)
	{
		Histogram2d<T> hist(binsX, binsY, minX, maxX, minY, maxY);
		if (i < 0)
		{
			hist.increment_cpu(beginX, std::prev(endX, -i),
				std::next(beginY, -i), endY);
		}
		else if (i > 0)
		{
			hist.increment_cpu(std::next(beginX, i), endX,
				beginY, std::prev(endY, i));
		}
		else // Should not be necessary but better be explicit.
		{
			hist.increment_cpu(beginX, endX,
				beginY, endY);
		}
		output[(i - shift_from) / shift_step] = *hist.calculate_mutual_information();
	}
}

template<typename T>
void shifted_mutual_information_with_bootstrap(
	const int shift_from, const int shift_to,
	const int binsX, const int binsY,
	const T minX, const T maxX, const T minY, const T maxY,
	const int* beginX, const int* endX,
	const int* beginY, const int* endY,
	int nr_samples, int nr_repetitions,
	const int shift_step,
	T* output)
{
	size_t sizeX = std::distance(beginX, endX);
	size_t sizeY = std::distance(beginY, endY);
	check_shifted_mutual_information(sizeX, sizeY, shift_from, shift_to,
		binsX, binsY, minX, maxX, minY, maxY, shift_step);
	if (nr_samples < 1)
		throw std::logic_error("For bootstrapping you need a minimum of one sample.");
	if (nr_repetitions < 1)
		throw std::logic_error("There needs to be at least one repetition of the bootstrapping process.");
#pragma omp parallel for
	for (int i = shift_from; i <= shift_to; i += shift_step)
	{
		unsigned int seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
		std::mt19937 rgen(seed);
		for (int j = 0; j < nr_repetitions; ++j)
		{
			T mi;
			if (i < 0)
			{
				mi = bootstrapped_mi<T>(beginX, std::prev(endX, -i),
					std::next(beginY, -i), endY,
					binsX, binsY, minX, maxX, minY, maxY, nr_samples, rgen);
			}
			else if (i > 0)
			{
				mi = bootstrapped_mi<T>(std::next(beginX, i), endX,
					beginY, std::prev(endY, i),
					binsX, binsY, minX, maxX, minY, maxY, nr_samples, rgen);
			}
			else // Should not be necessary but better be explicit.
			{
				mi = bootstrapped_mi<T>(beginX, endX,
					beginY, endY,
					binsX, binsY, minX, maxX, minY, maxY, nr_samples, rgen);
			}
			output[((i - shift_from) / shift_step) * nr_repetitions + j] = mi;
		}
	}
}