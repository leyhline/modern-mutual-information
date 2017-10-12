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

// TODO Check return codes for errors!!!

CudaMI::CudaMI(const int shift_from, const int shift_to,
			   const unsigned int binsX, const unsigned int binsY,
		       const float minX, const float maxX,
			   const float minY, const float maxY,
		       const float* const dataX,
			   const float* const dataY, const size_t data_size)
	: byte_size(data_size * sizeof(float)), result_size(shift_to - shift_from)
{
	h_result = (float*)malloc(result_size * sizeof(float));
	cudaMalloc((void**)&d_X, byte_size);
	cudaMalloc((void**)&d_Y, byte_size);
	cudaMemcpy(d_X, dataX, byte_size, cudaMemcpyHostToDevice);
	cudaMemcpy(d_Y, dataY, byte_size, cudaMemcpyHostToDevice);
}

CudaMI::~CudaMI()
{
	free(h_result);
	cudaFree(d_X);
	cudaFree(d_Y);
}

const float* CudaMI::shifted_mutual_information()
{
	// TODO Code comes here.
	return h_result;
}

int CudaMI::getSizeOfShiftedArray() const
{
	return result_size;
}
