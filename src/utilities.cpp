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
#include <random>
#include <chrono>

#include "utilities.h"
#include "Histogram2d.h"

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
		if (   x >= minX
			&& x <= maxX
			&& y >= minY
			&& y <= maxY)
		{
			int indexX;
			if (x == maxX)
				indexX = binsX - 1;
			else
				indexX = (x - minX) / (maxX - minX) * binsX;
			int indexY;
			if (y == maxY)
				indexY = binsY - 1;
			else
				indexY = (y - minY) / (maxY - minY) * binsY;
			result[i] = index_pair {indexX, indexY}; // I hope this is allowed.
		}
		else
		{
			result[i] = index_pair {INT_MAX, INT_MAX};
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
	std::vector<T> result((shift_to - shift_from) / shift_step + 1);
	#pragma omp parallel for
	for (int i = shift_from; i <= shift_to; i += shift_step)
	{
		Histogram2d<T> hist(binsX, binsY, minX, maxX, minY, maxY);
		if (i < 0)
		{
			hist.increment_cpu(indicesX.begin(), std::prev(indicesX.end(), -i),
					           std::next(indicesY.begin(), -i), indicesY.end());
		}
		else if (i > 0)
		{
			hist.increment_cpu(std::next(indicesX.begin(), i), indicesX.end(),
							   indicesY.begin(), std::prev(indicesY.end(), i));
		}
		else // Should not be necessary but better be explicit.
		{
			hist.increment_cpu(indicesX.begin(), indicesX.end(),
							   indicesY.begin(), indicesY.end());
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
inline T calc_mean(Iterator begin, Iterator end)
{
	T mean = 0;
	size_t size = std::distance(begin, end);
	T total = T(size);
	for (size_t i = 0; i < size; ++i)
	{
		mean += begin[i] / total;
	}
	return mean;
}

template<typename T, typename Iterator>
std::vector<T> shifted_mutual_information_with_bootstrap(
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
	std::vector<int> indicesX = calculate_indices_1d(binsX, minX, maxX, beginX, endX);
	std::vector<int> indicesY = calculate_indices_1d(binsY, minY, maxY, beginY, endY);
	std::vector<T> result((shift_to - shift_from) / shift_step + 1);
	#pragma omp parallel for
	for (int i = shift_from; i <= shift_to; i += shift_step)
	{
		auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
		std::mt19937 rgen(seed);
		std::vector<T> mis(nr_repetitions);
		for (int j = 0; j < nr_repetitions; ++j)
		{
			T mi;
			if (i < 0)
			{
				mi = bootstrapped_mi<T>(indicesX.begin(), std::prev(indicesX.end(), -i),
										std::next(indicesY.begin(), -i), indicesY.end(),
										binsX, binsY, minX, maxX, minY, maxY, nr_samples, rgen);
			}
			else if (i > 0)
			{
				mi = bootstrapped_mi<T>(std::next(indicesX.begin(), i), indicesX.end(),
										indicesY.begin(), std::prev(indicesY.end(), i),
										binsX, binsY, minX, maxX, minY, maxY, nr_samples, rgen);
			}
			else // Should not be necessary but better be explicit.
			{
				mi = bootstrapped_mi<T>(indicesX.begin(), indicesX.end(),
										indicesY.begin(), indicesY.end(),
										binsX, binsY, minX, maxX, minY, maxY, nr_samples, rgen);
			}
			mis[j] = mi;
		}
		T mean = calc_mean<T>(mis.begin(), mis.end()); 
		result[(i - shift_from) / shift_step] = mean;
	}
	return result;
}

template<typename T>
void shifted_mutual_information(
	    const int shift_from, const int shift_to,
		const int binsX, const int binsY,
		const T minX, const T maxX, const T minY, const T maxY,
		const T* beginX, const T* endX,
		const T* beginY, const T* endY,
		const int shift_step,
		T* output)
{
	std::vector<T> result = shifted_mutual_information<T>(
		shift_from, shift_to, binsX, binsY, minX, maxX, minY, maxY,
		beginX, endX, beginY, endY, shift_step);
	for (int i = 0, end = result.size(); i < end; ++i)
	{
		output[i] = result[i];
	}
}

template<typename T>
void shifted_mutual_information_with_bootstrap(
		const int shift_from, const int shift_to,
		const int binsX, const int binsY,
		const T minX, const T maxX, const T minY, const T maxY,
		const T* beginX, const T* endX,
		const T* beginY, const T* endY,
		int nr_samples, int nr_repetitions,
		const int shift_step,
		T* output)
{
	std::vector<T> result = shifted_mutual_information_with_bootstrap<T>(
		shift_from, shift_to, binsX, binsY, minX, maxX, minY, maxY,
		beginX, endX, beginY, endY, nr_samples, nr_repetitions, shift_step);
	for (int i = 0, end = result.size(); i < end; ++i)
	{
		output[i] = result[i];
	}
}

// Compile for these instances:
typedef std::mt19937 RNG;
//		For float:
// 			Take vector iterator.
typedef std::vector<float>::iterator fvec_iter;
template std::vector<int> calculate_indices_1d(int, float, float, fvec_iter, fvec_iter);
template std::vector<index_pair> calculate_indices_2d(
		int, int, float, float, float, float, fvec_iter, fvec_iter, fvec_iter, fvec_iter);
template std::vector<float> shifted_mutual_information(
		int, int, int, int, float, float, float, float, fvec_iter, fvec_iter, fvec_iter, fvec_iter, int);
template std::vector<float> shifted_mutual_information_with_bootstrap(
		int, int, int, int, float, float, float, float, fvec_iter, fvec_iter, fvec_iter, fvec_iter, int, int, int);
template float bootstrapped_mi(fvec_iter, fvec_iter, fvec_iter, fvec_iter, int, int, float, float, float, float, int, RNG&);
//			Take normal C-style arrays (i.e. pointers)
template std::vector<int> calculate_indices_1d(int, float, float, const float*, const float*);
template std::vector<index_pair> calculate_indices_2d(
		int, int, float, float, float, float, const float*, const float*, const float*, const float*);
template std::vector<float> shifted_mutual_information(
		int, int, int, int, float, float, float, float, const float*, const float*, const float*, const float*, int);
template void shifted_mutual_information(
		int, int, int, int, float, float, float, float, const float*, const float*, const float*, const float*, int, float*);
template std::vector<float> shifted_mutual_information_with_bootstrap(
		int, int, int, int, float, float, float, float, const float*, const float*, const float*, const float*, int, int, int);
template void shifted_mutual_information_with_bootstrap(
	int, int, int, int, float, float, float, float, const float*, const float*, const float*, const float*, int, int, int, float*);
template float bootstrapped_mi(const float*, const float*, const float*, const float*, int, int, float, float, float, float, int, RNG&);
