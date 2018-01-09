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

#include "Histogram1d.h"

#include <vector>
#include <stdexcept>
#include <cstddef>

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

// Compile for these instances:
//		For float:
template class Histogram1d<float>;
//			For vector iterators:
typedef std::vector<float>::iterator fvec_iter;
template void Histogram1d<float>::calculate_cpu(fvec_iter, fvec_iter);
typedef std::vector<int>::iterator sizevec_iter;
template void Histogram1d<float>::increment_cpu(sizevec_iter, sizevec_iter);
//			For C-style arrays (i.e. pointers)
template void Histogram1d<float>::calculate_cpu(const float*, const float*);
template void Histogram1d<float>::increment_cpu(const int*, const int*);
//		For double:
template class Histogram1d<double>;
//			For vector iterators:
typedef std::vector<double>::iterator dvec_iter;
template void Histogram1d<double>::calculate_cpu(dvec_iter, dvec_iter);
template void Histogram1d<double>::increment_cpu(sizevec_iter, sizevec_iter);
//			For C-style arrays (i.e. pointers)
template void Histogram1d<double>::calculate_cpu(const double*, const double*);
template void Histogram1d<double>::increment_cpu(const int*, const int*);