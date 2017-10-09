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

#pragma once

#include "Histogram1d.h"

#include <vector>
#include <cstddef>
#include <utility>
#include <memory>

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
	Histogram2d(unsigned int binsX, unsigned int binsY,
			    T minX, T maxX,
			    T minY, T maxY);

	/**
	 * Calculate the histogram single-threaded on the CPU.
	 */
	template<typename Iterator>
	void calculate_cpu(const Iterator beginX, const Iterator endX,
					   const Iterator beginY, const Iterator endY);

	/*
	 * Get bin count of x-axis as specified in constructor.
	 */
	unsigned int getBinsX() const;

	/*
	 * Get bin count of y-axis as specified in constructor.
	 */
	unsigned int getBinsY() const;

	/**
	 * Get total number of values inserted into histogram.
	 */
	unsigned int getCount() const;

	/**
	 * Get reference to histogram vector.
	 */
	const std::vector<std::vector<unsigned int>>& getHistogram() const;

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
	const unsigned int binsX;
	const unsigned int binsY;
	unsigned int count;
	T minX;
	T maxX;
	T minY;
	T maxY;
	std::vector<std::vector<unsigned int>> H;
	std::unique_ptr<Histogram1d<T>> hist1dX;
	std::unique_ptr<Histogram1d<T>> hist1dY;
	std::unique_ptr<T> mutual_information;

	/**
	 * Transfer function for actually doing the insertion into H.
	 * Use x and y parameters to calculate index at which to increment.
	 */
	void transfer(T x, T y);

	void check_constructor() const;
};