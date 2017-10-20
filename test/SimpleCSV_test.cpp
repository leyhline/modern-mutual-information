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
	SimpleCSV<float> csv("test/data/SimpleCSV_data1.csv");
	auto data = csv.getData();
	REQUIRE( data.size() == 3 );
	CHECK( data[0] == 1 );
	CHECK( data[1] == 2 );
	CHECK( data[2] == 3 );
}



