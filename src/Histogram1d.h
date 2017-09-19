/**
 * (c) Copyright 2017, Thomas Leyh <leyht@informatik.uni-freiburg.de>
 */

#pragma once
#include <vector>


template<typename T>
	// requires Integral<T>
class Histogram1d
{
public:
	Histogram1d(unsigned int bins, const std::vector<T>& data);

	Histogram1d(unsigned int bins, const std::vector<T>& data, T min, T max);

	~Histogram1d();

	void calculate_cpu();

	unsigned int getBins() const;

	const std::vector<unsigned int>& getHistogram() const;

	T getMax() const;

	T getMin() const;

private:
	unsigned int bins;
	const std::vector<T>& data;
	T min;
	T max;
	std::vector<unsigned int> H;

	void transfer(T value);
};
