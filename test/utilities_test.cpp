/**
* Copyright 2018, University of Freiburg
* Optophysiology Lab.
* Thomas Leyh <thomas.leyh@mailbox.org>
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

#include <catch.hpp>
#include <cmath>
#include <climits>
#include <algorithm>
#include "../src/utilities.h"

TEST_CASE( "Test calculation of indices in 1 dimension.", "[calculate_indices_1d]" )
{
	std::vector<float> input(1000);
	for (int i = 0; i < 1000; ++i)
	{
		input[i] = float(i) - 500.f;
	}
	input[123] = NAN;
	auto indices = calculate_indices_1d(10, -500.f, 499.f, input.begin(), input.end());
	REQUIRE( indices.size() == 1000 );
	CHECK( indices[0] == 0 );
	CHECK( indices[23] == 0 );
	CHECK( indices[99] == 0 );
	CHECK( indices[100] == 1 );
	CHECK( indices[123] == INT_MAX );
	CHECK( indices[199] == 1 );
	CHECK( indices[990] == 9 );
	CHECK( indices[999] == 9 );
}

TEST_CASE( "Test calculation of indices in 2 dimensions.", "[calculate_indices_2d]" )
{
	std::vector<float> inputX(800);
	for (int i = 0; i < 800; ++i)
	{
		inputX[i] = float(i) - 500.f;
	}
	std::vector<float> inputY(800);
	for (int i = 0; i < 800; ++i)
	{
		inputY[i] = float(i) - 400.f;
	}
	inputX[123] = NAN;
	auto indices = calculate_indices_2d(10, 10,
			inputX.front(), inputX.back(), inputY.front(), inputY.back(),
			inputX.begin(), inputX.end(), inputY.begin(), inputY.end());
	REQUIRE( indices.size() == 800 );
	CHECK( indices[0].first == 0 );
	CHECK( indices[0].second == 0 );
	CHECK( indices[79].first == 0 );
	CHECK( indices[79].second == 0 );
	CHECK( indices[80].first == 1 );
	CHECK( indices[80].second == 1 );
	CHECK( indices[123].first == INT_MAX );
	CHECK( indices[123].second == INT_MAX );
	CHECK( indices[799].first == 9 );
	CHECK( indices[799].second == 9 );
}

TEST_CASE( "Shifted mutual information on sinoid data." "[shifted_mutual_information]")
{
	// Fill a vector with sinoid data.
	std::vector<float> data(1000);
	float value = 0;
	for (auto& d : data)
	{
		d = std::sin(value);
		value += 0.01f;
	}
	auto result = shifted_mutual_information(-100, 100, 10, 10, -1.f, 1.f, -1.f, 1.f,
											 data.begin(), data.end(), data.begin(), data.end());
	REQUIRE( result.size() == 201 );
	auto maximum = std::max_element(result.begin(), result.end());
	REQUIRE( std::distance(result.begin(), maximum) == 100 ); // Should be exactly the middle.
	// And now check for symmetries.
	auto rbegin = result.rbegin();
	for (auto begin = result.begin(); begin != maximum; ++begin)
	{
		CHECK( *begin == Approx(*rbegin) );
		++rbegin;
	}

	auto result_s3 = shifted_mutual_information(-100, 100, 10, 10, -1.f, 1.f, -1.f, 1.f,
												data.begin(), data.end(), data.begin(), data.end(), 3);
	REQUIRE( result_s3.size() == 67 );
	auto maximum_s3 = std::max_element(result_s3.begin(), result_s3.end());
	REQUIRE( std::distance(result_s3.begin(), maximum_s3) == 33 );

	// And now test writing directly to array.
	float array_result[201];
	auto indices = calculate_indices_1d(10, -1.f, 1.f, data.begin(), data.end());
	int* data_ptr = indices.data();
	shifted_mutual_information(-100, 100, 10, 10, -1.f, 1.f, -1.f, 1.f,
							   data_ptr, data_ptr + 1000, data_ptr, data_ptr + 1000, 1, array_result);
	for (int i = 0; i < 201; ++i)
	{
		CHECK( array_result[i] == result[i] );
	}
}

TEST_CASE( "Exact test on small set of triangle data." "[shifted_mutual_information_triangle]")
{
	std::vector<float> data {0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f,
							 9.f, 8.f, 7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f};
	auto result = shifted_mutual_information(-5, 5, 5, 5, 0.f, 10.f, 0.f, 10.f,
						 data.begin(), data.end(), data.begin(), data.end());
	REQUIRE( result.size() == 11 );
	CHECK( result[0] == Approx(0.954434f) );
	CHECK( result[1] == Approx(1.30985808f) );
	CHECK( result[2] == Approx(0.68497745f) );
	CHECK( result[3] == Approx(1.38138049f) );
	CHECK( result[4] == Approx(1.0854753f) );
	CHECK( result[5] == Approx(2.31566788f) );
	CHECK( result[6] == Approx(1.0854753f) );
	CHECK( result[7] == Approx(1.38138049f) );
	CHECK( result[8] == Approx(0.68497745f) );
	CHECK( result[9] == Approx(1.30985808f) );
	CHECK( result[10] == Approx(0.954434f) );
}

template<typename T, typename Iterator>
inline T calc_mean(Iterator begin, Iterator end)
{
	T mean = 0;
	size_t size = std::distance(begin, end);
	for (size_t i = 0; i < size; ++i)
	{
		mean += begin[i];
	}
	return mean / T(size);
}

template<typename T, typename Iterator>
inline T calc_std(Iterator begin, Iterator end, T mean)
{
	T std = 0;
	size_t size = std::distance(begin, end);
	for (size_t i = 0; i < size; ++i)
	{
		T temp = begin[i] - mean;
		std += temp * temp;
	}
	return std::sqrt(std / T(size));
}

TEST_CASE("Shifted mutual information with bootstrapping on sinoid data." "[shifted_mutual_information_with_bootstrap]")
{
	std::vector<float> data(1000);
	float value = 0;
	for (auto& d : data)
	{
		d = std::sin(value);
		value += 0.01f;
	}
	auto result = shifted_mutual_information_with_bootstrap(-100, 100, 10, 10, -1.f, 1.f, -1.f, 1.f,
		data.begin(), data.end(), data.begin(), data.end(), 100, 100);
	REQUIRE(result.size() == 201);
	REQUIRE(result[0].size() == 100);
	std::vector<float> mean(201);
	std::vector<float> std(201);
	for (int i = 0; i < 201; ++i)
	{
		mean[i] = calc_mean<float>(result[i].begin(), result[i].end());
	}
	for (int i = 0; i < 201; ++i)
	{
		std[i] = calc_std<float>(result[i].begin(), result[i].end(), mean[i]);
	}
}