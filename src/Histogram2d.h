/**
 * (c) Copyright 2017, Thomas Leyh <leyht@informatik.uni-freiburg.de>
 */

#pragma once

#include "Histogram1d.h"

#include <vector>
#include <cstddef>
#include <utility>

template<typename T>
	// requires Integral<T>
class Histogram2d
{
public:
	Histogram2d(unsigned int binsX, unsigned int binsY,
			const std::vector<T>& dataX, const std::vector<T>& dataY);

	Histogram2d(unsigned int binsX, unsigned int binsY,
			const std::vector<T>& dataX, T minX, T maxX,
			const std::vector<T>& dataY, T minY, T maxY);

	void calculate_cpu();

	unsigned int getBinsX() const;
	unsigned int getBinsY() const;

	unsigned int getCount() const;

	const std::vector<std::vector<unsigned int>>& getHistogram() const;

	std::pair<Histogram1d<T>, Histogram1d<T>> reduce1d() const;

	T getMinX() const;
	T getMaxX() const;
	T getMinY() const;
	T getMaxY() const;

private:
	const unsigned int binsX;
	const unsigned int binsY;
	unsigned int count;
	const std::vector<T>& dataX;
	const std::vector<T>& dataY;
	T minX;
	T maxX;
	T minY;
	T maxY;
	std::vector<std::vector<unsigned int>> H;
	std::size_t min_length;

	void transfer(T x, T y);

	void check_constructor() const;
};
