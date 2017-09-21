/**
 * (c) Copyright 2017, Thomas Leyh <leyht@informatik.uni-freiburg.de>
 */

#include <catch.hpp>
#include <vector>
#include "../src/Histogram2d.h"


TEST_CASE( "Test 2D Histogram with two vectors with linear values.", "[Histogram2d]" )
{
	std::vector<float> input(1000);
	for (int i = 0; i < 1000; ++i)
	{
		input[i] = float(i) - 500.f;
	}
	Histogram2d<float> hist(10, 10, input, input);
}
