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

#pragma once

#include <string>
#include <fstream>
#include <vector>
#include "ISimpleFile.h"

/**
 * A very simple class for reading and writing from binary files.
 */
template<typename T>
	// requires Integral<T>
class SimpleBinaryFile : public ISimpleFile<T>
{
public:
	/**
	 * Constructor.
	 * @param path Which file to parse.
     * @param Precision of processed file (32 or 64 allowed)
	 */
	SimpleBinaryFile(const std::string& path, int precision);

	/**
	 * Getter for resulting data parsed from file.
	 */
	std::vector<T>& getData() override;

	/**
	 * Write data to file.
	 */
	void writeData(const std::vector<T>& data_to_write) override;

private:
	const std::string path;
    int precision;
	std::vector<T> data;

    template<typename Prec>
	void parse_file(const std::string& input);
};
