/**
 * (c) Copyright 2017, Thomas Leyh <leyht@informatik.uni-freiburg.de>
 */

#include "Histogram1d.h"

#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cstddef>

template<typename T>
Histogram1d<T>::Histogram1d(unsigned int bins, const std::vector<T>& data)
	: bins(bins), data(data), count(0)
{
	auto bounds = std::minmax_element(data.begin(), data.end());
	min = *bounds.first;
	max = *bounds.second;
	check_constructor();
	H.resize(bins, 0);
}

template<typename T>
Histogram1d<T>::Histogram1d(unsigned int bins, const std::vector<T>& data, T min, T max)
	: bins(bins), data(data), min(min), max(max), count(0)
{
	check_constructor();
	H.resize(bins, 0);
}

template<typename T>
Histogram1d<T>::Histogram1d(unsigned int bins, const std::vector<T>& data, T min, T max,
		std::vector<unsigned int> H, unsigned int count)
		: bins(bins), data(data), min(min), max(max), count(count), H(H)
{
	if (H.size() != bins)
		throw std::invalid_argument("Argument bins has to be of same size as H vector.");
	check_constructor();
}

template<typename T>
void Histogram1d<T>::calculate_cpu()
{
	for (std::size_t i = 0; i < data.size(); ++i)
		transfer(data[i]);
}

template<typename T>
unsigned int Histogram1d<T>::getBins() const
{
	return bins;
}

template<typename T>
unsigned int Histogram1d<T>::getCount() const
{
	return count;
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
	if (value >= min && value < max)
	{
		T normalized = (value - min) / (max - min);
		unsigned int index = normalized * bins;  // Implicit conversion to integer.
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
	if (data.size() == 0)
		throw std::logic_error("data must not be an empty vector.");
	if (bins < 1)
		throw std::invalid_argument("There must be at least one bin.");
}

// Compile for these instances.
template class Histogram1d<float>;
template class Histogram1d<double>;
