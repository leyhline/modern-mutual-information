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
#include "../src/containers.h"

/**
 * A class for 2D-histogram calculation.
 * Usable for all integral types.
 */
template<typename Precision>
	// requires Integral<Precision>
class Histogram2d
{
public:
	Histogram2d(int binsX, int binsY, DataRange2D<Precision> dataRange);

	/**
	 * Calculate the histogram single-threaded on the CPU.
	 * @param beginX Iterator to the beginning of the first data container.
	 * @param endX Iterator to the end of the first data container.
	 * @param beginY Iterator to the beginning of the second data container.
	 * @param endY Iterator to the end of the second data container.
	 */
	template<typename Iterator>
	void calculate(const Iterator beginX, const Iterator endX,
				   const Iterator beginY, const Iterator endY);

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
	void increment(const Iterator beginX, const Iterator endX,
				   const Iterator beginY, const Iterator endY);

	/**
	 * Add another histogram to this one, meaning the values at corresponding
	 * indices are summed up. Histograms need to have same bin size.
	 * @param histogramToAdd needs to have same bin size on both axes as original histogram.
	 */
	void add(const Histogram2d<Precision>& histogramToAdd);

	int getBinsX() const;

	int getBinsY() const;

	int getElementCount() const;

	const std::vector<std::vector<int>>& getHistogramData() const;

	/**
	 * Calculate two 1-D histograms from 2-D histogram.
	 * This is done lazily, meaning the calculation is only done the first time
	 * the method is called.
	 * @param force Don't use lazy evaluation and force (re)calculation.
	 * @return A pair of pointers to the corresponding Histogram1d classes.
	 */
	std::pair<const Histogram1d<Precision>*, const Histogram1d<Precision>*> reduce1d(bool force=false);

	DataRange2D<Precision> getDataRange() const;

	/**
	 * Calculate mutual information.
	 * This works lazily like reduce1d. The actual calculation is only done the
	 * first time this method is called.
	 * @param force Don't use lazy evaluation and force (re)calculation.
	 * @return A pointer to the mutual information value.
	 */
	const Precision* calculateMutualInformation(bool force=false);

private:
	const int binsX;
	const int binsY;
	int elementCount;
	const DataRange2D<Precision> dataRange;
	std::vector<std::vector<int>> histData;
	std::unique_ptr<Histogram1d<Precision>> histogramX;
	std::unique_ptr<Histogram1d<Precision>> histogramY;
	std::unique_ptr<Precision> mutualInformation;

	/**
	 * Transfer function for actually doing the insertion into H.
	 * Use x and y parameters to calculate index at which to increment.
	 */
	void inline transfer(const Precision x, const Precision y);

	void checkConstructor() const;
};


//////////////////
/// IMPLEMENTATION
//////////////////

template<typename Precision>
Histogram2d<Precision>::Histogram2d(int binsX, int binsY, DataRange2D<Precision> dataRange)
	: binsX(binsX), binsY(binsY), elementCount(0), dataRange(dataRange)
{
	checkConstructor();
	histData.resize(binsX, std::vector<int>(binsY, 0));
}

template<typename Precision>
template<typename Iterator>
void Histogram2d<Precision>::calculate(const Iterator beginX, const Iterator endX,
							   const Iterator beginY, const Iterator endY)
{
	if (std::distance(beginX, endX) != std::distance(beginY, endY))
		throw std::logic_error("Containers referenced by iterators must have the same size.");
	for (auto iX = beginX, iY = beginY; iX != endX; ++iX, ++iY)
	{
		transfer(*iX, *iY);
	}
}

template<typename Precision>
template<typename Iterator>
void Histogram2d<Precision>::increment(const Iterator beginX, const Iterator endX,
						       const Iterator beginY, const Iterator endY)
{
	if (std::distance(beginX, endX) != std::distance(beginY, endY))
		throw std::logic_error("Containers referenced by iterators must have the same size.");
	for (auto iX = beginX, iY = beginY; iX != endX; ++iX, ++iY)
	{
		if (*iX < binsX && *iY < binsY)
		{
			++histData[*iX][*iY];
			++elementCount;
		}
	}
}

template<typename Precision>
void Histogram2d<Precision>::add(const Histogram2d<Precision>& histogramToAdd)
{
	if (   histogramToAdd.getBinsX() != binsX
		|| histogramToAdd.getBinsY() != binsY)
		throw std::logic_error("Unable to sum histograms with different bin size.");
	auto newHistData = histogramToAdd.getHistogramData();
	for (int x = 0; x < binsX; ++x)
	{
		for (int y = 0; y < binsY; ++y)
		{
			histData[x][y] += newHistData[x][y];
		}
	}
	elementCount += histogramToAdd.getElementCount();
}

template<typename Precision>
int Histogram2d<Precision>::getBinsX() const
{
	return binsX;
}

template<typename Precision>
int Histogram2d<Precision>::getBinsY() const
{
	return binsY;
}

template<typename Precision>
int Histogram2d<Precision>::getElementCount() const
{
	return elementCount;
}

template<typename Precision>
const std::vector<std::vector<int> >& Histogram2d<Precision>::getHistogramData() const
{
	return histData;
}

template<typename Precision>
DataRange2D<Precision> Histogram2d<Precision>::getDataRange() const
{
	return dataRange;
}

template<typename Precision>
void Histogram2d<Precision>::transfer(const Precision x, const Precision y)
{
	Precision minX = dataRange.minX;
	Precision maxX = dataRange.maxX;
	Precision minY = dataRange.minY;
	Precision maxY = dataRange.maxY;
	if (   x >= minX
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
		++histData[indexX][indexY];
		++elementCount;
	}
}

template<typename Precision>
std::pair<const Histogram1d<Precision>*, const Histogram1d<Precision>*> Histogram2d<Precision>::reduce1d(bool force /* false */)
{
	if (force || !histogramX || !histogramY)
	{
		std::vector<int> histDataX(binsX, 0);
		std::vector<int> histDataY(binsY, 0);
		for (int x = 0; x < binsX; ++x)
		{
			for (int y = 0; y < binsY; ++y)
			{
				histDataX[x] += histData[x][y];
				histDataY[y] += histData[x][y];
			}
		}
		DataRange<Precision> dataRangeX(dataRange.minX, dataRange.maxX);
		DataRange<Precision> dataRangeY(dataRange.minY, dataRange.maxY);
		histogramX.reset(new Histogram1d<Precision>(binsX, dataRangeX, histDataX, elementCount));
		histogramY.reset(new Histogram1d<Precision>(binsY, dataRangeY, histDataY, elementCount));
	}
	return std::pair<const Histogram1d<Precision>*, const Histogram1d<Precision>*>(histogramX.get(), histogramY.get());
}

template<typename Precision>
void Histogram2d<Precision>::checkConstructor() const
{
	if (binsX < 1)
		throw std::invalid_argument("There must be at least one binX.");
	if (binsY < 1)
		throw std::invalid_argument("There must be at least one binY.");
}

template <typename Precision>
const Precision* Histogram2d<Precision>::calculateMutualInformation(bool force /* false */)
{
	if (force || !mutualInformation)
	{
		Precision mi = 0;
		auto histogram1d_pair = reduce1d(force);
		for (int x = 0; x < binsX; ++x)
		{
			for (int y = 0; y < binsY; ++y)
			{
				if (histData[x][y] > 0)
				{
					Precision p_xy = Precision(histData[x][y]) / elementCount;
					Precision p_x = Precision(histogram1d_pair.first->getHistogramData()[x]) / elementCount;
					Precision p_y = Precision(histogram1d_pair.second->getHistogramData()[y]) / elementCount;
					mi += p_xy * std::log2(p_xy / (p_x * p_y));
				}
			}
		}
		mutualInformation.reset(new Precision(mi));
	}
	return mutualInformation.get();
}