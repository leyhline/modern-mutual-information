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

#include <vector>
#include "mex.h"
#include "build/modern_mutual_information.h"

/**
 * @param nlhs Size of the output array, should be 1
 * @param plhs The output is just one mxArray holding the shifted mutual information
 * @param nrhs Number of input arguments; should be 6
 * @param prhs The input arguments, specifically:
 *             [0] [shift_from, shift_to]
 *             [1] [binsX, binsY]
 *             [2] [minX maxX minY maxY]
 *             [3] input data on x axis
 *             [4] input data on y axis
 *             [5] shift_step (size of steps when shifting)
 * See: <https://de.mathworks.com/help/matlab/matlab_external/standalone-example.html>
 */
void mexFunction(int nlhs,              // Number of output (left-side) arguments, or the size of the plhs array.
                 mxArray *plhs[],       // Array of output arguments.
                 int nrhs,              // Number of input (right-side) arguments, or the size of the prhs array.
                 const mxArray *prhs[]) // Array of input arguments.
{
    // Check number of arguments.
    if (nrhs != 6)
        mexErrMsgIdAndTxt("modern_mutual_information:nrhs", "Five inputs required.");
    if (nlhs != 1)
        mexErrMsgIdAndTxt("modern_mutual_information:nlhs", "One output required.");

    // Get input arguments.
    if (!mxIsInt32(prhs[0]))
        mexErrMsgIdAndTxt("modern_mutual_information:notInt", "shift_from and shift_to need to be of type Int32");
    if (   mxGetM(prhs[0]) != 1
        || mxGetN(prhs[0]) != 2)
        mexErrMsgIdAndTxt("modern_mutual_information:wrongDimensions", "shift values need to be a 1x2 matrix.");
    int* shift = mxGetData(prhs[0]);

    if (!mxIsInt32(prhs[1]))
        mexErrMsgIdAndTxt("modern_mutual_information:notInt", "binsX and binsY need to be of type Int32");
    if (   mxGetM(prhs[1]) != 1
        || mxGetN(prhs[1]) != 2)
        mexErrMsgIdAndTxt("modern_mutual_information:wrongDimensions", "bin sizes need to be a 1x2 matrix.");
    int* bin_sizes = mxGetData(prhs[1]);

    if (!mxIsSingle(prhs[3]))
        mexErrMsgIdAndTxt("modern_mutual_information:notSingle", "min and max values need to be of type single (float).");
    if (   mxGetM(prhs[3]) != 1
        || mxGetN(prhs[3]) != 4)
        mexErrMsgIdAndTxt("modern_mutual_information:wrongDimensions", "minmax_values need to be a 1x4 matrix.");
    float* minmax = mxGetData(prhs[2]);

    if (!mxIsSingle(prhs[3]))
        mexErrMsgIdAndTxt("modern_mutual_information:notSingle", "Input matrix must be type single (float).");
    if (mxGetM(prhs[3]) != 1)
        mexErrMsgIdAndTxt("modern_mutual_information:wrongDimensions", "Input matrix must be a row vector.");
    float* dataX = mxGetData(prhs[3]);
    mwSize dataX_size = mxGetN(prhs[3]);

    if (!mxIsSingle(prhs[4]))
        mexErrMsgIdAndTxt("modern_mutual_information:notSingle", "Input matrix must be type single (float).");
    if (mxGetM(prhs[4]) != 1)
        mexErrMsgIdAndTxt("modern_mutual_information:wrongDimensions", "Input matrix must be a row vector.");
    float* dataY = mxGetData(prhs[4]);
    mwSize dataY_size = mxGetN(prhs[4]);

    if (!mxIsScalar(prhs[5]))
        mexErrMsgIdAndTxt("modern_mutual_information:notScalar", "shift_step needs to be a scalar value.");
    if (!mxIsInt32(prhs[5]))
        mexErrMsgIdAndTxt("modern_mutual_information:notInt", "shift_step needs to be of type Int32.");
    int shift_step = mxGetScalar(prhs[5]); 

    // And now do the calculation.
    std::vector<float> result = shifted_mutual_information<float>(
            shift[0], shift[1],
            bin_sizes[0], bin_sizes[1],
            minmax[0], minmax[1], minmax[2], minmax[3],
            dataX, dataX + dataX_size,
            dataY, dataY + dataY_size,
            shift_step);

    // And not let's write the result back.
    plhs[0] = mxCreateNumericMatrix(1, result.size(), mxClassIDFromClassName("single"), mxREAL);
    float* outMatrix = mxGetData(plhs[0]);
    for (mwSize i = 0, end = result.size(); i < end; ++i)
    {
        outMatrix[i] = result[i];
    }
}