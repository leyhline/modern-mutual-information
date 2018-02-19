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
#include <utility>
#include <memory>
#include "utilities.h"
#include <stdexcept>
#include <iterator>
#include <cmath>
#include "Histogram1d.h"

/**
 * A class for 2D-histogram calculation.
 * Usable for all integral types.
 */
template<typename T>
	// requires Integral<T>
class Histogram2d
{
public:
	/**
	 * Constructor for known range of values.
	 * If there are values outside of [min,max] they are ignored at insertion.
	 * @param binsX Number of bins on histogram's x-axis.
	 * @param binsY Number of bins on histogram's y-axis.
	 * @param minX Minimum value in first data vector.
	 * @param maxX Maximum value in first data vector.
	 * @param minY Minimum value in second data vector.
	 * @param maxY Maximum value in second data vector.
	 */
	Histogram2d(int binsX, int binsY,
			    T minX, T maxX,
			    T minY, T maxY);

	/**
	 * Calculate the histogram single-threaded on the CPU.
	 * @param beginX Iterator to the beginning of the first data container.
	 * @param endX Iterator to the end of the first data container.
	 * @param beginY Iterator to the beginning of the second data container.
	 * @param endY Iterator to the end of the second data container.
	 */
	template<typename Iterator>
	void calculate_cpu(const Iterator beginX, const Iterator endX,
					   const Iterator beginY, const Iterator endY);

	/**
	 * Given an iterator of pairs of indices (specifically the index_pair struct defined
	 * in utilities.h) this method allows for incrementing the histogram at these
	 * indices' positions.
	 * If one of the index positions is smaller than the corresponding bin number
	 * no insertion takes place.
	 * @param begin Iterator to the beginning of the index data.
	 * @param end: Iterator to the end of the index data.
	 */
	template<typename Iterator>
	void increment_cpu(const Iterator begin, const Iterator end);

	/**
	 * Given two iterators with index positions of same size the histogram will
	 * be incremented at these positions.
	 * If one of the index positions is smaller than the corresponding bin number
	 * no insertion takes place.
	 * @param beginX Iterator to the beginning of the indices corresponding to the x-axis.
	 * @param endX Iterator to the end of the indices corresponding to the x-axis.
	 * @param beginY Iterator to the beginning of the indices corresponding to the y-axis.
	 * @param endY Iterator to the end of the indices corresponding to the y-axis.
	 */
	template<typename Iterator>
	void increment_cpu(const Iterator beginX, const Iterator endX,
					   const Iterator beginY, const Iterator endY);

	/**
	 * Increment histogram at specified position by one.
	 */
	void increment_at(int iX, int iY);

	/**
	 * Add another histogram to this one, meaning the values at corresponding
	 * indices are summed up. Histograms need to have same bin size.
	 * @param histogram_to_add needs to have same bin size on both axes as original histogram.
	 */
	void add(const Histogram2d<T>& histogram_to_add);

	/**
	 * Get bin count of x-axis as specified in constructor.
	 */
	int getBinsX() const;

	/**
	 * Get bin count of y-axis as specified in constructor.
	 */
	int getBinsY() const;

	/**
	 * Get total number of values inserted into histogram.
	 */
	int getCount() const;

	/**
	 * Get reference to histogram vector.
	 */
	const std::vector<std::vector<int>>& getHistogram() const;

	/**
	 * Calculate two 1-D histograms from 2-D histogram.
	 * This is done lazily, meaning the calculation is only done the first time
	 * the method is called.
	 * @param force Don't use lazy evaluation and force (re)calculation.
	 * @return A pair of pointers to the corresponding Histogram1d classes.
	 */
	std::pair<const Histogram1d<T>*, const Histogram1d<T>*> reduce1d(bool force=false);

	/**
	 * Get (supposed) minimum value of the first data vector.
	 */
	T getMinX() const;

	/**
	 * Get (supposed) maximum value of the first data vector.
	 */
	T getMaxX() const;

	/**
	 * Get (supposed) minimum value of the second data vector.
	 */
	T getMinY() const;

	/**
	 * Get (supposed) maximum value of the second data vector.
	 */
	T getMaxY() const;

	/**
	 * Calculate mutual information.
	 * This works lazily like reduce1d. The actual calculation is only done the
	 * first time this method is called.
	 * @param force Don't use lazy evaluation and force (re)calculation.
	 * @return A pointer to the mutual information value.
	 */
	const T* calculate_mutual_information(bool force=false);

private:
	const int binsX;
	const int binsY;
	int count;
	const T minX;
	const T maxX;
	const T minY;
	const T maxY;
	std::vector<std::vector<int>> H;
	std::unique_ptr<Histogram1d<T>> hist1dX;
	std::unique_ptr<Histogram1d<T>> hist1dY;
	std::unique_ptr<T> mutual_information;

	/**
	 * Transfer function for actually doing the insertion into H.
	 * Use x and y parameters to calculate index at which to increment.
	 */
	void transfer(const T x, const T y);

	void check_constructor() const;
};


//////////////////
/// IMPLEMENTATION
//////////////////

template<typename T>
Histogram2d<T>::Histogram2d(int binsX, int binsY,
	T minX, T maxX,
	T minY, T maxY)
	: binsX(binsX), binsY(binsY), count(0), minX(minX), maxX(maxX),
	minY(minY), maxY(maxY)
{
	check_constructor();
	H.resize(binsX, std::vector<int>(binsY, 0));
}

template<typename T>
template<typename Iterator>
void Histogram2d<T>::calculate_cpu(const Iterator beginX, const Iterator endX,
	const Iterator beginY, const Iterator endY)
{
	if (std::distance(beginX, endX) != std::distance(beginY, endY))
		throw std::logic_error("Containers referenced by iterators must have the same size.");
	for (auto iX = beginX, iY = beginY; iX != endX; ++iX, ++iY)
	{
		transfer(*iX, *iY);
	}
}

template<typename T>
template<typename Iterator>
void Histogram2d<T>::increment_cpu(const Iterator begin, const Iterator end)
{
	for (auto index = begin; index != end; ++index)
	{
		if (index->first  < binsX
			&& index->second < binsY)
		{
			++H[index->first][index->second];
			++count;
		}
	}
}

template<typename T>
template<typename Iterator>
void Histogram2d<T>::increment_cpu(const Iterator beginX, const Iterator endX,
	const Iterator beginY, const Iterator endY)
{
	if (std::distance(beginX, endX) != std::distance(beginY, endY))
		throw std::logic_error("Containers referenced by iterators must have the same size.");
	for (auto iX = beginX, iY = beginY; iX != endX; ++iX, ++iY)
	{
		if (*iX < binsX && *iY < binsY)
		{
			++H[*iX][*iY];
			++count;
		}
	}
}

template<typename T>
void Histogram2d<T>::increment_at(int iX, int iY)
{
	if (iX < binsX && iY < binsY)
	{
		++H[iX][iY];
		++count;
	}
}

template<typename T>
void Histogram2d<T>::add(const Histogram2d<T>& histogram_to_add)
{
	if (histogram_to_add.getBinsX() != binsX
		|| histogram_to_add.getBinsY() != binsY)
		throw std::logic_error("Unable to sum histograms with different bin size.");
	auto h2 = histogram_to_add.getHistogram();
	for (int x = 0; x < binsX; ++x)
	{
		for (int y = 0; y < binsY; ++y)
		{
			H[x][y] += h2[x][y];
		}
	}
	count += histogram_to_add.getCount();
}

template<typename T>
int Histogram2d<T>::getBinsX() const
{
	return binsX;
}

template<typename T>
int Histogram2d<T>::getBinsY() const
{
	return binsY;
}

template<typename T>
int Histogram2d<T>::getCount() const
{
	return count;
}

template<typename T>
const std::vector<std::vector<int> >& Histogram2d<T>::getHistogram() const
{
	return H;
}

template<typename T>
T Histogram2d<T>::getMinX() const
{
	return minX;
}

template<typename T>
T Histogram2d<T>::getMaxX() const
{
	return maxX;
}

template<typename T>
T Histogram2d<T>::getMinY() const
{
	return minY;
}

template<typename T>
T Histogram2d<T>::getMaxY() const
{
	return maxY;
}

template<typename T>
void Histogram2d<T>::transfer(const T x, const T y)
{
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
		++H[indexX][indexY];
		++count;
	}
}

template<typename T>
std::pair<const Histogram1d<T>*, const Histogram1d<T>*> Histogram2d<T>::reduce1d(bool force /* false */)
{
	if (force || !hist1dX || !hist1dY)
	{
		std::vector<int> vecX(binsX, 0);
		std::vector<int> vecY(binsY, 0);
		for (int x = 0; x < binsX; ++x)
		{
			for (int y = 0; y < binsY; ++y)
			{
				vecX[x] += H[x][y];
				vecY[y] += H[x][y];
			}
		}
		hist1dX.reset(new Histogram1d<T>(binsX, minX, maxX, vecX, count));
		hist1dY.reset(new Histogram1d<T>(binsY, minY, maxY, vecY, count));
	}
	return std::pair<const Histogram1d<T>*, const Histogram1d<T>*>(hist1dX.get(), hist1dY.get());
}

template<typename T>
void Histogram2d<T>::check_constructor() const
{
	if (minX >= maxX)
		throw std::logic_error("minX has to be smaller than maxX.");
	if (minY >= maxY)
		throw std::logic_error("minY has to be smaller than maxY.");
	if (binsX < 1)
		throw std::invalid_argument("There must be at least one binX.");
	if (binsY < 1)
		throw std::invalid_argument("There must be at least one binY.");
}

template <typename T>
const T* Histogram2d<T>::calculate_mutual_information(bool force /* false */)
{
	if (force || !mutual_information)
	{
		T mi = 0;
		auto h = reduce1d(force);
		for (int x = 0; x < binsX; ++x)
		{
			for (int y = 0; y < binsY; ++y)
			{
				if (H[x][y] > 0)
				{
					T p_xy = T(H[x][y]) / count;
					T p_x = T(h.first->getHistogram()[x]) / count;
					T p_y = T(h.second->getHistogram()[y]) / count;
					mi += p_xy * std::log2(p_xy / (p_x * p_y));
				}
			}
		}
		mutual_information.reset(new T(mi));
	}
	return mutual_information.get();
}