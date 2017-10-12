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

#include <vector>
#include <iterator>
#include <cstddef>

/**
 * Calculates the histogram indices of a certain data container.
 * @param bins Number of bins for the imaginary histogram.
 * @param min Minimum value in data container.
 * @param max Maximum value in data container.
 * @param begin Iterator to the beginning of the data.
 * @param end Iterator to the end of the data.
 * @return A vector with the same size as the data holding the index positions.
 * 	       If there is a value outside the [min,max] range UINT_MAX is inserted
 * 	       at this position.
 */
template<typename T, typename Iterator>
std::vector<int> calculate_indices_1d(
		const int bins,
		const T min, const T max,
		const Iterator begin, const Iterator end);

/**
 * Small struct for simply holding two index values.
 */
struct index_pair
{
	int first;
	int second;
};

/**
 * Calculate indices of a 2d histogram for a certain data container.
 * @param binsX Number of bins on imaginary histogram's x-axis.
 * @param binsY Number of bins on imaginary histogram's y-axis.
 * @param minX Minimum value in first data container.
 * @param maxX Maximum value in first data container.
 * @param minY Minimum value in second data container.
 * @param maxY Maximum value in second data container.
 * @param beginX Iterator to the beginning of the first data container.
 * @param endX Iterator to the end of the first data container.
 * @param beginY Iterator to the beginning of the second data container.
 * @param endY Iterator to the end of the second data container.
 * 			   Both containers must have the same size.
 * @return Vector of index_pair structs. The vector has the same size as the data.
 *         If there is a value outside the [min,max] range index_pair {UINT_MAX, UINT_MAX}
 *         is inserted at this position.
 */
template<typename T, typename Iterator>
std::vector<index_pair> calculate_indices_2d(
		const int binsX, const int binsY,
		const T minX, const T maxX,
		const T minY, const T maxY,
		const Iterator beginX, const Iterator endX,
		const Iterator beginY, const Iterator endY);

/**
 * Calculate a certain range of mutual information by shifting the second data container
 * in relation to the first one.
 * No shift:
 * 		|--------------------| dataX
 * 		|--------------------| dataY
 * Negative shift:
 * 		|--------------------| dataX
 * 	|--------------------| 	   dataY
 * Positive shift:
 * 		|--------------------| 	   dataX
 * 			|--------------------| dataY
 * @param shift_from Negative (or positive) value for specifying the largest shift to the left.
 * @param shift_to Positive (of negative) value for specifying the largest shift to the right.
 * 	      Has to be greater than shift_from.
 * @param minX Minimum value in first data container.
 * @param maxX Maximum value in first data container.
 * @param minY Minimum value in second data container.
 * @param maxY Maximum value in second data container.
 * @param beginX Iterator to the beginning of the first data container.
 * @param endX Iterator to the end of the first data container.
 * @param beginY Iterator to the beginning of the second data container.
 * @param endY Iterator to the end of the second data container.
 * 			   Both containers must have the same size.
 * @param shift_step (Optional) Specifies the steps between shifts. Default = 1.
 * @return Vector with size (shift_to - shift_from) holding the mutual information for each shift.
 * 		   Might be smaller if shift_step is specified.
 */
template<typename T, typename Iterator>
std::vector<T> shifted_mutual_information(
		const int shift_from, const int shift_to,
		const int binsX, const int binsY,
		const T minX, const T maxX, const T minY, const T maxY,
		const Iterator beginX, const Iterator endX,
		const Iterator beginY, const Iterator endY,
		const int shift_step = 1);
