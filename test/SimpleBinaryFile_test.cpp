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
#include "../src/SimpleBinaryFile.h"

TEST_CASE( "Write and read a 32bit binary file.", "[SimpleBinaryFileFloat]" )
{
    std::vector<float> data1;
    data1.push_back(1.f);
    data1.push_back(2.f);
    data1.push_back(3.f);
    SimpleBinaryFile<float> file1("test/SimpleBinaryFile_gen1.bin", PREC_32);
    file1.writeData(data1);
    auto file1_data = file1.getData();
    REQUIRE( file1_data.size() == 3 );
    CHECK ( file1_data[0] == 1.f );
    CHECK ( file1_data[1] == 2.f );
    CHECK ( file1_data[2] == 3.f );

    SimpleBinaryFile<float> same_file1("test/SimpleBinaryFile_gen1.bin", PREC_32);
    auto same_file1_data = same_file1.getData();
    REQUIRE( same_file1_data.size() == 3 );
    for (int i = 0, end = file1_data.size(); i < end; ++i)
        CHECK( file1_data[i] == same_file1_data[i] );

    SimpleBinaryFile<float> file2("test/SimpleBinaryFile_data1.bin", PREC_32);
    auto file2_data = file2.getData();
    REQUIRE( file2_data.size() == 1000 );
    CHECK( file2_data[0] == 0.f );
    CHECK( file2_data[999] == 999.f );
}

TEST_CASE( "Write and read a 64bit binary file.", "[SimpleBinaryFileDouble]" )
{
    SimpleBinaryFile<double> file1("test/SimpleBinaryFile_data2.bin", PREC_64);
    auto file1_data = file1.getData();
    REQUIRE( file1_data.size() == 1000 );
    CHECK( file1_data[0] == 0. );
    CHECK( file1_data[999] == 999. );

    SimpleBinaryFile<float> file1f("test/SimpleBinaryFile_data2.bin", PREC_64);
    auto file1f_data = file1f.getData();
    REQUIRE( file1f_data.size() == 1000 );
    CHECK( file1f_data[0] == 0.f );
    CHECK( file1f_data[999] == 999.f );
}