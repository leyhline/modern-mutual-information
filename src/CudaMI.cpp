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
#include <cuda_runtime_api.h>
#include <cuda.h>

#include "CudaMI.h"

CudaMI::CudaMI(const float* const data, const size_t data_size)
	: h_result(nullptr), byte_size(data_size * sizeof(float))
{
	cudaMalloc((void**)&d_memory, byte_size);
	cudaMemcpy(d_memory, data, byte_size, cudaMemcpyHostToDevice);
}

CudaMI::~CudaMI()
{
	cudaFree(d_memory);
}
