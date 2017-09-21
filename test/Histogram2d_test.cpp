/**
 * (c) Copyright 2017, Thomas Leyh <leyht@informatik.uni-freiburg.de>
 */

#include <catch.hpp>
#include <vector>
#include "../src/Histogram2d.h"


TEST_CASE( "Test 2D Histogram with two vectors with linear values.", "[Histogram2d]" )
{
	std::vector<float> inputX(1000);
	for (int i = 0; i < 1000; ++i)
	{
		inputX[i] = float(i) - 500.f;
	}
	std::vector<float> inputY(800);
	for (int i = 0; i < 800; ++i)
	{
		inputY[i] = float(i) - 400.f;
	}
	Histogram2d<float> hist(10, 10, inputX, inputY);
	REQUIRE( hist.getBinsX() == 10 );
	REQUIRE( hist.getBinsX() == 10 );
	REQUIRE( hist.getMinX() == Approx(-500.f) );
	REQUIRE( hist.getMaxX() == Approx(299.f) );
	REQUIRE( hist.getMinY() == Approx(-400.f) );
	REQUIRE( hist.getMaxY() == Approx(399.f) );
	CHECK( hist.getCount() == 0 );
	hist.calculate_cpu();
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
	auto hists1d = hist.reduce1d();
	for (auto value : hists1d.first.getHistogram())
	{
		CHECK( value == 80 );
	}
	for (auto value : hists1d.second.getHistogram())
	{
		CHECK( value == 80 );
	}
}
