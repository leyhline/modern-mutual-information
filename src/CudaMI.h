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

#include <cstddef>

class CudaMI
{
public:
	CudaMI(const int shift_from, const int shift_to,
		   const int binsX, const int binsY,
		   const float minX, const float maxX,
		   const float minY, const float maxY,
		   const float* const dataX,
		   const float* const dataY, const size_t data_size);

	~CudaMI();

	const float* shifted_mutual_information();

	int getSizeOfShiftedArray() const;

private:
	static constexpr int block_size {128};
	const size_t data_size;
	const int result_size;
	bool calculation_done;
	float* h_result;
	float* d_X;
	float* d_Y;
};
