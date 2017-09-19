/**
 * (c) Copyright 2017, Thomas Leyh <leyht@informatik.uni-freiburg.de>
 */

#include "Histogram1d.h"
#include <vector>
#include <algorithm>
#include <iterator>
#include <functional>
#include <cmath>


template<typename T>
Histogram1d<T>::Histogram1d(unsigned int bins, const std::vector<T>& data)
	: bins(bins), data(data)
{
	auto bounds = std::minmax(data.begin(), data.end());
	min = *bounds.first;
	max = *bounds.second;
	H.resize(bins, 0);
}

template<typename T>
Histogram1d<T>::Histogram1d(unsigned int bins, const std::vector<T>& data, T min, T max)
	: bins(bins), data(data), min(min), max(max)
{
	H.resize(bins, 0);
}

template<typename T>
Histogram1d<T>::~Histogram1d()
{
}

template<typename T>
void Histogram1d<T>::calculate_cpu()
{
	for (unsigned int i = 0; i < data.size(); ++i)
		transfer(data[i]);
}

template<typename T>
unsigned int Histogram1d<T>::getBins() const
{
	return bins;
}

template<typename T>
const std::vector<unsigned int>& Histogram1d<T>::getHistogram() const
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
void Histogram1d<T>::transfer(T value)
{
	if (value >= min && value <= max)
	{
		unsigned int index = std::lround((value - min) / (max - min) * (bins - 1));
		++H[index];
	}
}


template class Histogram1d<float>;
