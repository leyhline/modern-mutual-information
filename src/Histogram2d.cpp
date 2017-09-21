/**
 * (c) Copyright 2017, Thomas Leyh <leyht@informatik.uni-freiburg.de>
 */

#include "Histogram2d.h"
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cstddef>
#include <iterator>


template<typename T>
Histogram2d<T>::Histogram2d(unsigned int binsX, unsigned int binsY,
		const std::vector<T>& dataX, const std::vector<T>& dataY)
		: binsX(binsX), binsY(binsY), dataX(dataX), dataY(dataY), count(0)
{
	min_length = std::min(dataX.size(), dataY.size());
	auto boundsX = std::minmax_element(dataX.begin(), std::next(dataX.begin(), min_length));
	minX = *boundsX.first;
	maxX = *boundsX.second;
	auto boundsY = std::minmax_element(dataY.begin(), std::next(dataY.begin(), min_length));
	minY = *boundsY.first;
	maxY = *boundsY.second;
	check_constructor();
	H.resize(binsX, std::vector<unsigned int>(binsY, 0));
}

template<typename T>
Histogram2d<T>::Histogram2d(unsigned int binsX, unsigned int binsY,
		const std::vector<T>& dataX, T minX, T maxX,
		const std::vector<T>& dataY, T minY, T maxY)
		: binsX(binsX), binsY(binsY), dataX(dataX), dataY(dataY), count(0)
{
	check_constructor();
	min_length = std::min(dataX.size(), dataY.size());
	H.resize(binsX, std::vector<unsigned int>(binsY, 0));
}

template<typename T>
void Histogram2d<T>::calculate_cpu()
{
	for (std::size_t i = 0; i < min_length; ++i)
		transfer(dataX[i], dataY[i]);
}

template<typename T>
unsigned int Histogram2d<T>::getBinsX() const
{
	return binsX;
}

template<typename T>
unsigned int Histogram2d<T>::getBinsY() const
{
	return binsY;
}

template<typename T>
unsigned int Histogram2d<T>::getCount() const
{
	return count;
}

template<typename T>
const std::vector<std::vector<unsigned int> >& Histogram2d<T>::getHistogram() const
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
void Histogram2d<T>::transfer(T x, T y)
{
	if (   x >= minX
		&& x <  maxX
		&& y >= minY
		&& y <  maxY)
	{
		unsigned int indexX = (x - minX) / (maxX - minX) * binsX;
		unsigned int indexY = (y - minY) / (maxY - minY) * binsY;
		++H[indexX][indexY];
		++count;
	}
}

template<typename T>
void Histogram2d<T>::check_constructor() const
{
	if (minX >= maxX)
		throw std::logic_error("minX has to be smaller than maxX.");
	if (minY >= maxY)
		throw std::logic_error("minY has to be smaller than maxY.");
	if (dataX.size() == 0)
		throw std::logic_error("dataX must not be an empty vector.");
	if (dataY.size() == 0)
		throw std::logic_error("dataY must not be an empty vector.");
	if (binsX < 1)
		throw std::invalid_argument("There must be at least one binX.");
	if (binsY < 1)
		throw std::invalid_argument("There must be at least one binY.");
}

// Compile for these instances.
template class Histogram2d<float>;
template class Histogram2d<double>;
