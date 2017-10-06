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

#include <catch.hpp>
#include <vector>
#include "../src/Histogram2d.h"


TEST_CASE( "Test 2D Histogram with two vectors with linear values.", "[Histogram2d]" )
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
	Histogram2d<float> hist(10, 10, inputX.front(), inputX.back(), inputY.front(), inputY.back());
	REQUIRE( hist.getBinsX() == 10 );
	REQUIRE( hist.getBinsX() == 10 );
	REQUIRE( hist.getMinX() == Approx(-500.f) );
	REQUIRE( hist.getMaxX() == Approx(299.f) );
	REQUIRE( hist.getMinY() == Approx(-400.f) );
	REQUIRE( hist.getMaxY() == Approx(399.f) );
	CHECK( hist.getCount() == 0 );
	hist.calculate_cpu(inputX.begin(), inputX.end(), inputY.begin(), inputY.end());
	CHECK( hist.getCount() == 800 );
	auto result = hist.getHistogram();
	for (int x = 0; x < 10; ++x)
	{
		for (int y = 0; y < 10; ++y)
		{
			if (x == y)
				CHECK( result[x][y] == 80 );
			else
				CHECK( result[x][y] == 0 );
		}
	}
	// Check if calculation works.
	auto hists1d = hist.reduce1d();
	for (auto value : hists1d.first->getHistogram())
	{
		CHECK( value == 80 );
	}
	for (auto value : hists1d.second->getHistogram())
	{
		CHECK( value == 80 );
	}
	// Check if lazy evaluation works.
	auto hist1dlazy = hist.reduce1d();
	CHECK ( hists1d.first == hist1dlazy.first );
	CHECK ( hists1d.second == hist1dlazy.second );
	auto mi = hist.calculate_mutual_information();
	CHECK ( *mi == Approx(3.3219) );
	auto mi_lazy = hist.calculate_mutual_information();
	CHECK ( *mi_lazy == Approx(3.3219) );
	CHECK ( mi == mi_lazy );
}
