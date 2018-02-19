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

#pragma once

#include <cstddef>

// Kernel computes exactly 10x10 Histogram.
#define BINS 10

class CudaMI
{
public:
	static constexpr int block_size {64};

	CudaMI(const int shift_from, const int shift_to,
		   const float minX, const float maxX,
		   const float minY, const float maxY,
		   const float* const dataX,
		   const float* const dataY, const int data_size);

	~CudaMI();

	const float* shifted_mutual_information();

	int getSizeOfShiftedArray() const;

private:
	const int shift_from;
	const int shift_to;
	const float minX;
	const float maxX;
	const float minY;
	const float maxY;
	const int data_size;
	const int result_size;
	bool calculation_done;
	float* h_result;
	float* d_result;
	int* d_X;
	int* d_Y;

	void check_constructor() const;
};
