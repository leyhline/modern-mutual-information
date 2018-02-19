/**
* Copyright 2018, University of Freiburg
* Optophysiology Lab.
* Thomas Leyh <leyh.thomas@web.de>
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
#include "../src/Histogram2d.h"
#include "../src/containers.h"

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
	DataRange2D<float> dataRange(inputX.front(), inputX.back(), inputY.front(), inputY.back());
	Histogram2d<float> hist(10, 10, dataRange);
	REQUIRE( hist.getBinsX() == 10 );
	REQUIRE( hist.getBinsX() == 10 );
	DataRange2D<float> returnedDataRange = hist.getDataRange();
	REQUIRE( returnedDataRange.minX == Approx(-500.f) );
	REQUIRE( returnedDataRange.maxX == Approx(299.f) );
	REQUIRE( returnedDataRange.minY == Approx(-400.f) );
	REQUIRE( returnedDataRange.maxY == Approx(399.f) );
	CHECK( hist.getElementCount() == 0 );
	hist.calculate(inputX.begin(), inputX.end(), inputY.begin(), inputY.end());
	CHECK( hist.getElementCount() == 800 );
	auto result = hist.getHistogramData();
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
	for (auto value : hists1d.first->getHistogramData())
	{
		CHECK( value == 80 );
	}
	for (auto value : hists1d.second->getHistogramData())
	{
		CHECK( value == 80 );
	}
	// Check if lazy evaluation works.
	auto hist1dlazy = hist.reduce1d();
	CHECK ( hists1d.first == hist1dlazy.first );
	CHECK ( hists1d.second == hist1dlazy.second );
	auto mi = hist.calculateMutualInformation();
	CHECK ( *mi == Approx(3.3219) );
	auto mi_lazy = hist.calculateMutualInformation();
	CHECK ( *mi_lazy == Approx(3.3219) );
	CHECK ( mi == mi_lazy );
}

TEST_CASE( "Add two simple histograms", "[Histogram2d_add]" )
{
	DataRange2D<float> dataRange(0.f, 1.f, 0.f, 1.f);
	Histogram2d<float> hist1(3, 3, dataRange);
	Histogram2d<float> hist2(3, 3, dataRange);
	int input1x[3] = { 0, 1, 2 };
	int input1y[3] = { 0, 0, 0 };
	int input2x[4] = { 0, 0, 0, 1 };
	int input2y[4] = { 0, 1, 2, 1 };
	hist1.increment(input1x, input1x + 3, input1y, input1y + 3);
	hist2.increment(input2x, input2x + 4, input2y, input2y + 4);
	REQUIRE( hist1.getElementCount() == 3 );
	REQUIRE( hist2.getElementCount() == 4 );
	hist1.add(hist2);
	REQUIRE( hist1.getElementCount() == 7 );
	auto h1 = hist1.getHistogramData();
	CHECK( h1[0][0] == 2 );
	CHECK( h1[1][0] == 1 );
	CHECK( h1[0][1] == 1 );
	CHECK( h1[1][1] == 1 );
}