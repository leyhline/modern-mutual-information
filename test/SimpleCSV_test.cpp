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
#include "../src/SimpleCSV.h"

TEST_CASE( "Test reading of CSV files with space delimiter", "[SimpleCSV]" )
{
	SimpleCSV<float> csv1("test/SimpleCSV_data1.csv");
	auto data1 = csv1.getData();
	REQUIRE( data1.size() == 3 );
	CHECK( data1[0] == 1 );
	CHECK( data1[1] == 2 );
	CHECK( data1[2] == 3 );

	SimpleCSV<float> csv2("test/SimpleCSV_random1.csv");
	auto random1 = csv2.getData();
	REQUIRE( random1.size() == 10000 );
	CHECK(random1[0] == Approx(0.32243f) );

	SimpleCSV<float> csv3("test/SimpleCSV_random2.csv");
	auto random2 = csv3.getData();
	REQUIRE( random2.size() == 10000 );
}



