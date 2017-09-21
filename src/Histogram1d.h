/**
 * (c) Copyright 2017, Thomas Leyh <leyht@informatik.uni-freiburg.de>
 */

#pragma once
#include <vector>


/**
 * A class for 1D-histogram calculation.
 * Usable for all integral types.
 */
template<typename T>
	// requires Integral<T>
class Histogram1d
{
public:
	/**
	 * Constructor for unknown range of values.
	 * If range is known consider the other constructor because of better performance.
	 * @param bins Number of bins.
	 * @param data Reference to original data vector.
	 */
	Histogram1d(unsigned int bins, const std::vector<T>& data);

	/**
	 * Constructor for known range of values.
	 * If there are values outside of [min,max] they are ignored at insertion.
	 * @param bins Number of bins.
	 * @param data Reference to original 1D data vector.
	 * @param min Minimum value in data array.
	 * @param max Maximum value in data array.
	 */
	Histogram1d(unsigned int bins, const std::vector<T>& data, T min, T max);

	/**
	 * Construct class from already available histogram data.
	 * @param bins Number of bins.
	 * @param data Reference to original 1D data vector.
	 * @param min Minimum value in data array.
	 * @param max Maximum value in data array.
	 * @param H Histogram vector holding the bins.
	 * @param Total number of values in histogram.
	 */
	Histogram1d(unsigned int bins, const std::vector<T>& data, T min, T max,
			std::vector<unsigned int> H, unsigned int count);

	/**
	 * Calculate the histogram single-threaded on the CPU.
	 */
	void calculate_cpu();

	/**
	 * Get bin count as specified in constructor.
	 */
	unsigned int getBins() const;

	/**
	 * Get total number of values inserted into histogram.
	 */
	unsigned int getCount() const;

	/**
	 * Get reference to histogram vector.
	 */
	const std::vector<unsigned int>& getHistogram() const;

	/**
	 * Get (supposed) maximum value of the data.
	 */
	T getMax() const;

	/**
	 * Get (supposed) minimum value of the data.
	 */
	T getMin() const;

private:
	const unsigned int bins;
	unsigned int count;
	const std::vector<T>& data;
	T min;
	T max;
	std::vector<unsigned int> H;

	/**
	 * Transfer function for actually doing the insertion into H.
	 * @param value Increment histogram for this value.
	 */
	void transfer(T value);

	void check_constructor() const;
};
