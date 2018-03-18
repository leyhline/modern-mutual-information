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
#include <vector>
#include <algorithm>
#include "../src/Histogram1d.h"
#include "../src/utilities.h"


TEST_CASE( "Test Histogram for linear values -500 to 500 with 10 bins.", "[Histogram1d]" )
{
	std::vector<float> input(1000);
	for (int i = 0; i < 1000; ++i)
	{
		input[i] = float(i) - 500.f;
	}
	Histogram1d<float> hist(10, -500.f, 499.f);
	REQUIRE( hist.getBins() == 10 );
	REQUIRE( hist.getMin() == Approx(-500.f) );
	REQUIRE( hist.getMax() == Approx(499.f) );
	CHECK( hist.getCount() == 0 );
	hist.calculate_cpu(input.begin(), input.end());
	auto result = hist.getHistogram();
	CHECK( hist.getCount() == 1000 );
	CHECK( result[0] == 100 );
	CHECK( result[1] == 100 );
	CHECK( result[2] == 100 );
	CHECK( result[3] == 100 );
	CHECK( result[4] == 100 );
	CHECK( result[5] == 100 );
	CHECK( result[6] == 100 );
	CHECK( result[7] == 100 );
	CHECK( result[8] == 100 );
	CHECK( result[9] == 100 );

	Histogram1d<float> hist_1bin(1, -500, 500);
	hist_1bin.calculate_cpu(input.begin(), input.end());
	auto result_1bin = hist_1bin.getHistogram();
	CHECK( result_1bin[0] == 1000 );

	Histogram1d<float> hist_copy(1, -500, 500, result_1bin, 1000);
	CHECK( hist_copy.getHistogram()[0] == 1000 );

	auto indices = calculate_indices_1d(10, -500.f, 499.f, input.begin(), input.end());
	Histogram1d<float> hist_with_indices(10, -500.f, 499.f);
	hist_with_indices.increment_cpu(indices.begin(), indices.end());
	auto result_with_indices = hist_with_indices.getHistogram();
	REQUIRE( hist_with_indices.getCount() == 1000 );
	REQUIRE( std::equal(result.begin(), result.end(), result_with_indices.begin()) );
}
