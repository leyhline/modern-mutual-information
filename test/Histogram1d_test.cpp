/**
 * (c) Copyright 2017, Thomas Leyh <leyht@informatik.uni-freiburg.de>
 */

#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <vector>
#include "../src/Histogram1d.h"


TEST_CASE( "Test Histogram for linear values -500 to 500 with 10 bins.", "[Histogram1d]" )
{
	std::vector<float> input(1000);
	for (int i = 0; i < 1000; ++i)
	{
		input[i] = float(i) - 500.f;
	}
	Histogram1d<float> hist(10, input);
	hist.calculate_cpu();
	auto result = hist.getHistogram();
	CHECK( hist.getBins() == 10 );
	CHECK( hist.getMin() == Approx(-500.f) );
	CHECK( hist.getMax() == Approx(499.f) );
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
}
