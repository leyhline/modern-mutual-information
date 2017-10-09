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

#include "Histogram2d.h"
#include "utilities.h"

#include <vector>
#include <stdexcept>
#include <cstddef>
#include <iterator>
#include <utility>
#include <cmath>

template<typename T>
Histogram2d<T>::Histogram2d(unsigned int binsX, unsigned int binsY,
							T minX, T maxX,
							T minY, T maxY)
		: binsX(binsX), binsY(binsY), minX(minX), maxX(maxX),
		                              minY(minY), maxY(maxY), count(0)
{
	check_constructor();
	H.resize(binsX, std::vector<unsigned int>(binsY, 0));
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
		++H[index->first][index->second];
	}
	count += std::distance(begin, end);
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
		&& x <= maxX
		&& y >= minY
		&& y <= maxY)
	{
		unsigned int indexX;
		if (x == maxX)
			indexX = binsX - 1;
		else
			indexX = (x - minX) / (maxX - minX) * binsX;
		unsigned int indexY;
		if (y == maxY)
			indexY = binsY - 1;
		else
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
		std::vector<unsigned int> vecX(binsX, 0);
		std::vector<unsigned int> vecY(binsY, 0);
		for (unsigned int x = 0; x < binsX; ++x)
		{
			for (unsigned int y = 0; y < binsY; ++y)
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
		for (unsigned int x = 0; x < binsX; ++x)
		{
			for (unsigned int y = 0; y < binsY; ++y)
			{
				if (H[x][y] > 0)
				{
					T p_xy = T(H[x][y]) / count;
					T p_x =  T(h.first->getHistogram()[x]) / count;
					T p_y = T(h.second->getHistogram()[y]) / count;
					mi += p_xy * std::log2(p_xy / (p_x * p_y));
				}
			}
		}
		mutual_information.reset(new T(mi));
	}
	return mutual_information.get();
}

// Compile for these instances.
template class Histogram2d<float>;
typedef std::vector<float>::iterator fvec_iter;
template void Histogram2d<float>::calculate_cpu(fvec_iter, fvec_iter, fvec_iter, fvec_iter);
typedef std::vector<index_pair>::iterator sizevec_iter;
template void Histogram2d<float>::increment_cpu(sizevec_iter, sizevec_iter);
