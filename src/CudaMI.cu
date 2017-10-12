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

#include <iterator>

#include "CudaMI.h"
#include "utilities.h"

// TODO Check return codes for errors!!!

CudaMI::CudaMI(const int shift_from, const int shift_to,
			   const int binsX, const int binsY,
		       const float minX, const float maxX,
			   const float minY, const float maxY,
		       const float* const dataX,
			   const float* const dataY, const size_t data_size)
	: data_size(data_size),
	  result_size(shift_to - shift_from),
	  calculation_done(false)
{
	h_result = (float*)malloc((size_t)result_size * sizeof(float));
	cudaMalloc((void**)&d_X, data_size * sizeof(int));
	cudaMalloc((void**)&d_Y, data_size * sizeof(int));
}

CudaMI::~CudaMI()
{
	free(h_result);
	cudaFree(d_X);
	cudaFree(d_Y);
}

const float* CudaMI::shifted_mutual_information()
{
	if (!calculation_done)
	{
		// TODO Code comes here.
		calculation_done = true;
	}
	return h_result;
}

int CudaMI::getSizeOfShiftedArray() const
{
	return result_size;
}
