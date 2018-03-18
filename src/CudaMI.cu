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

#include <iterator>

#include "CudaMI.h"
#include "utilities.h"

/// Calculate shifted mutual information.
/// Every kernel calculates one shift.
__global__ void shifted_mutual_information_kernel(
		const int shift_from, const int shift_to,
		const float minX, const float maxX,
		const float minY, const float maxY,
		const int* indicesX, const int* indicesY, int data_size,
		float* d_result)
{
	int index = blockDim.x * blockIdx.x + threadIdx.x;
	int result_size = shift_to - shift_from;
	if (index >= result_size)
		return;

	int hist[BINS][BINS];
	int count = 0;

	// Initialize whole histogram with 0.
	for (int x = 0; x < BINS; ++x)
	{
		for (int y = 0; y < BINS; ++y)
		{
			hist[x][y] = 0;
		}
	}

	// This is dynamic indexing and therefore we get into slow local memory. :(
	// Increment 2D histogram according to indices.
	int k_shift_from = shift_from + index;
	if (k_shift_from < 0)
	{
		for (int i = 0; i < (data_size - k_shift_from); ++i)
		{
			int x = indicesX[i];
			int y = indicesY[i + k_shift_from];
			if (x < BINS && y < BINS)
			{
				++hist[x][y];
				++count;
			}
		}
	}
	else
	{
		for (int i = 0; i < (data_size - k_shift_from); ++i)
		{
			int x = indicesX[i + k_shift_from];
			int y = indicesY[i];
			if (x < BINS && y < BINS)
			{
				++hist[x][y];
				++count;
			}
		}
	}

	// Calculate 1D histograms.
	int histX[BINS];
	for (int x = 0; x < BINS; ++x)
		histX[x] = 0;
	int histY[BINS];
	for (int y = 0; y < BINS; ++y)
		histY[y] = 0;
	for (int x = 0; x < BINS; ++x)
	{
		for (int y = 0; y < BINS; ++y)
		{
			histX[x] += hist[x][y];
			histY[y] += hist[x][y];
		}
	}

	// Calculate mutual information.
	float mi = 0;
	for (int x = 0; x < BINS; ++x)
	{
		for (int y = 0; y < BINS; ++y)
		{
			if (hist[x][y] > 0)
			{
				float p_xy = (float)(hist[x][y]) / count;
				float p_x =  (float)(histX[x]) / count;
				float p_y =  (float)(histY[y]) / count;
				mi += p_xy * log2(p_xy / (p_x * p_y));
			}
		}
	}

	d_result[index] = mi;
}

/// Calculate shifted mutual information.
/// Every kernel calculates one shift. Each kernel writes the results
/// of the histogram computation not to local but to fast shared memory.
__global__ void shifted_mutual_information_kernel_sharedmem(
		const int shift_from, const int shift_to,
		const float minX, const float maxX,
		const float minY, const float maxY,
		const int* indicesX, const int* indicesY, int data_size,
		float* d_result)
{
	int index = blockDim.x * blockIdx.x + threadIdx.x;
	int tid = threadIdx.x;
	int result_size = shift_to - shift_from;
	if (index >= result_size)
		return;

	__shared__ int hist[BINS][BINS][CudaMI::block_size];
	int count = 0;

	// Initialize whole histogram with 0.
	for (int x = 0; x < BINS; ++x)
	{
		for (int y = 0; y < BINS; ++y)
		{
			hist[x][y][tid] = 0;
		}
	}

	// Increment 2D histogram according to indices.
	// This in now in shared (instead of local) memory and faster.
	int k_shift_from = shift_from + index;
	if (k_shift_from < 0)
	{
		for (int i = 0; i < (data_size - k_shift_from); ++i)
		{
			int x = indicesX[i];
			int y = indicesY[i + k_shift_from];
			if (x < BINS && y < BINS)
			{
				++hist[x][y][tid];
				++count;
			}
		}
	}
	else
	{
		for (int i = 0; i < (data_size - k_shift_from); ++i)
		{
			int x = indicesX[i + k_shift_from];
			int y = indicesY[i];
			if (x < BINS && y < BINS)
			{
				++hist[x][y][tid];
				++count;
			}
		}
	}

	// Calculate 1D histograms.
	int histX[BINS];
	for (int x = 0; x < BINS; ++x)
		histX[x] = 0;
	int histY[BINS];
	for (int y = 0; y < BINS; ++y)
		histY[y] = 0;
	for (int x = 0; x < BINS; ++x)
	{
		for (int y = 0; y < BINS; ++y)
		{
			histX[x] += hist[x][y][tid];
			histY[y] += hist[x][y][tid];
		}
	}

	// Calculate mutual information.
	float mi = 0;
	for (int x = 0; x < BINS; ++x)
	{
		for (int y = 0; y < BINS; ++y)
		{
			if (hist[x][y][tid] > 0)
			{
				float p_xy = (float)(hist[x][y][tid]) / count;
				float p_x =  (float)(histX[x]) / count;
				float p_y =  (float)(histY[y]) / count;
				mi += p_xy * log2(p_xy / (p_x * p_y));
			}
		}
	}

	d_result[index] = mi;
}

// TODO Check return codes for errors!!!
// TODO Check if arguments are logical.

CudaMI::CudaMI(const int shift_from, const int shift_to,
		       const float minX, const float maxX,
			   const float minY, const float maxY,
		       const float* const dataX,
			   const float* const dataY, const int data_size)
	: shift_from(shift_from), shift_to(shift_to),
	  minX(minX), maxX(maxX),
	  minY(minY), maxY(maxY),
	  data_size(data_size), result_size(shift_to - shift_from), calculation_done(false)
{
	check_constructor();
	h_result = (float*)malloc(result_size * sizeof(float));
	cudaMalloc((void**)&d_result, result_size * sizeof(float));
	cudaMalloc((void**)&d_X, data_size * sizeof(int));
	cudaMalloc((void**)&d_Y, data_size * sizeof(int));
	// This can be done on the GPU later, too.
	auto indicesX = calculate_indices_1d(BINS, minX, maxX, dataX, dataX + data_size);
	auto indicesY = calculate_indices_1d(BINS, minY, maxY, dataY, dataY + data_size);
	cudaMemcpy(d_X, indicesX.data(), data_size * sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(d_Y, indicesY.data(), data_size * sizeof(int), cudaMemcpyHostToDevice);
}

CudaMI::~CudaMI()
{
	free(h_result);
	cudaFree(d_result);
	cudaFree(d_X);
	cudaFree(d_Y);
}

const float* CudaMI::shifted_mutual_information()
{
	if (!calculation_done)
	{
		// Ceiling of integer division result_size / block_size.
		int grid_size = ((result_size - 1) / block_size) + 1;
		shifted_mutual_information_kernel_sharedmem<<<grid_size, block_size>>>(
				shift_from, shift_to, minX, maxX, minY, maxY, d_X, d_Y, data_size, d_result);
		cudaMemcpy(h_result, d_result, result_size * sizeof(float), cudaMemcpyDeviceToHost);
		calculation_done = true;
	}
	return h_result;
}

int CudaMI::getSizeOfShiftedArray() const
{
	return result_size;
}

void CudaMI::check_constructor() const
{
	if (shift_from >= shift_to)
		throw std::logic_error("shift_from has to be smaller than shift_to.");
}
