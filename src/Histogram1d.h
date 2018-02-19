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
#include <stdexcept>
#include <cstddef>
#include "containers.h"

/**
 * A class for 1D-histogram calculation.
 * Usable for all integral types.
 */
template<typename Precision>
	// requires Integral<Precision>
class Histogram1d
{
public:
	Histogram1d(int nrBins, DataRange<Precision> dataRange);

	Histogram1d(int nrBins, DataRange<Precision> dataRange,
			    std::vector<int> histData, int elementCount);

	/**
	 * Calculate the histogram from data.
	 * @param begin Iterator to the beginning of the data.
	 * @param end Iterator to the end of the data.
	 */
	template<typename Iterator>
	void calculate(const Iterator begin, const Iterator end);

	/**
	 * Given an iterator (type: int) holding index positions, this methods increments
	 * the histogram for each value by one.
	 * If the index position is smaller than the bin number no insertion takes place.
	 * @param begin Iterator to the beginning of the index data.
	 * @param end: Iterator to the end of the index data.
	 */
	template<typename Iterator>
	void increment(const Iterator begin, const Iterator end);

	int getNrBins() const;

	int getElementCount() const;

	const std::vector<int>& getHistogramData() const;

	DataRange<Precision> getDataRange() const;

private:
	const int nrBins;
	int elementCount;
	const DataRange<Precision> dataRange;
	std::vector<int> histData;

	/**
	 * Transfer function for actually doing the insertion into H.
	 * @param value Increment histogram for this value.
	 */
	inline void transfer(const Precision value);

	void checkConstructor() const;
};


//////////////////
/// IMPLEMENTATION
//////////////////

template<typename Precision>
Histogram1d<Precision>::Histogram1d(int nrBins, DataRange<Precision> dataRange)
	: nrBins(nrBins), elementCount(0), dataRange(dataRange)
{
	checkConstructor();
	histData.resize(nrBins, 0);
}

template<typename Precision>
Histogram1d<Precision>::Histogram1d(int nrBins, DataRange<Precision> dataRange,
								    std::vector<int> histData, int elementCount)
	: nrBins(nrBins), elementCount(elementCount), dataRange(dataRange), histData(histData)
{
	checkConstructor();
	// Casting bins to size_type because it can't be negative thanks to checkConstructor method.
	if (histData.size() != std::vector<int>::size_type(nrBins))
		throw std::invalid_argument("Argument bins has to be of same size as HistData vector.");
}

template<typename Precision>
template<typename Iterator>
void Histogram1d<Precision>::calculate(const Iterator begin, const Iterator end)
{
	for (auto i = begin; i != end; ++i)
		transfer(*i);
}

template<typename Precision>
template<typename Iterator>
void Histogram1d<Precision>::increment(const Iterator begin, const Iterator end)
{
	for (auto index = begin; index != end; ++index)
	{
		if (*index < nrBins)
		{
			++histData[*index];
			++elementCount;
		}
	}
}

template<typename Precision>
int Histogram1d<Precision>::getNrBins() const
{
	return nrBins;
}

template<typename Precision>
int Histogram1d<Precision>::getElementCount() const
{
	return elementCount;
}

template<typename Precision>
const std::vector<int>& Histogram1d<Precision>::getHistogramData() const
{
	return histData;
}

template<typename Precision>
DataRange<Precision> Histogram1d<Precision>::getDataRange() const
{
	return dataRange;
}

template<typename Precision>
void Histogram1d<Precision>::transfer(const Precision value)
{
	Precision min = dataRange.min;
	Precision max = dataRange.max;
	if (value >= min && value < max)
	{
		Precision normalized = (value - min) / (max - min);
#pragma warning(suppress: 4244)
		int index = normalized * nrBins;  // Implicit conversion to integer.
		++histData[index];
		++elementCount;
	}
	else if (value == max)
	{
		++histData.back();
		++elementCount;
	}
}

template<typename Precision>
void Histogram1d<Precision>::checkConstructor() const
{
	if (nrBins < 1)
		throw std::invalid_argument("There must be at least one bin.");
}