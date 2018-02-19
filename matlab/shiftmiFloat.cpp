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

#include <vector>
#include "mex.h"
#include "../src/utilities.h"

/**
 * @param nlhs Size of the output array, should be 1
 * @param plhs The output is just one mxArray holding the shifted mutual information
 * @param nrhs Number of input arguments; should be 6
 * @param prhs The input arguments, specifically:
 *             [0] [shift_from, shift_to]
 *             [1] [binsX, binsY]
 *             [2] [minX maxX minY maxY]
 *             [3] histogram indices on x axis
 *             [4] histogram indices on y axis
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
        mexErrMsgIdAndTxt("modern_mutual_information:nrhs", "Six inputs required.");
    if (nlhs != 1)
        mexErrMsgIdAndTxt("modern_mutual_information:nlhs", "One output required.");

    // Get input arguments.
    if (!mxIsInt32(prhs[0]))
        mexErrMsgIdAndTxt("modern_mutual_information:notInt", "shift_from and shift_to need to be of type Int32");
    if (   mxGetM(prhs[0]) != 1
        || mxGetN(prhs[0]) != 2)
        mexErrMsgIdAndTxt("modern_mutual_information:wrongDimensions", "shift values need to be a 1x2 matrix.");
    int* shift = (int*)mxGetData(prhs[0]);

    if (!mxIsInt32(prhs[1]))
        mexErrMsgIdAndTxt("modern_mutual_information:notInt", "binsX and binsY need to be of type Int32");
    if (   mxGetM(prhs[1]) != 1
        || mxGetN(prhs[1]) != 2)
        mexErrMsgIdAndTxt("modern_mutual_information:wrongDimensions", "bin sizes need to be a 1x2 matrix.");
    int* bin_sizes = (int*)mxGetData(prhs[1]);

    if (!mxIsSingle(prhs[2]))
        mexErrMsgIdAndTxt("modern_mutual_information:notSingle", "min and max values need to be of type single (float).");
    if (   mxGetM(prhs[2]) != 1
        || mxGetN(prhs[2]) != 4)
        mexErrMsgIdAndTxt("modern_mutual_information:wrongDimensions", "minmax_values need to be a 1x4 matrix.");
    float* minmax = (float*)mxGetData(prhs[2]);

    if (!mxIsInt32(prhs[3]))
        mexErrMsgIdAndTxt("modern_mutual_information:notInt", "Input matrix must be type Int32.");
    if (mxGetM(prhs[3]) != 1)
        mexErrMsgIdAndTxt("modern_mutual_information:wrongDimensions", "Input matrix must be a row vector.");
    int* dataX = (int*)mxGetData(prhs[3]);
    mwSize dataX_size = mxGetN(prhs[3]);

    if (!mxIsInt32(prhs[4]))
        mexErrMsgIdAndTxt("modern_mutual_information:notInt", "Input matrix must be type Int32.");
    if (mxGetM(prhs[4]) != 1)
        mexErrMsgIdAndTxt("modern_mutual_information:wrongDimensions", "Input matrix must be a row vector.");
    int* dataY = (int*)mxGetData(prhs[4]);
    mwSize dataY_size = mxGetN(prhs[4]);

	if (dataX_size != dataY_size)
		mexErrMsgIdAndTxt("modern_mutual_information:unequalSize", "Both input data vectors need to have same size.");

    if (!mxIsScalar(prhs[5]))
        mexErrMsgIdAndTxt("modern_mutual_information:notScalar", "shift_step needs to be a scalar value.");
    if (!mxIsInt32(prhs[5]))
        mexErrMsgIdAndTxt("modern_mutual_information:notInt", "shift_step needs to be of type Int32.");
    int shift_step = (int)mxGetScalar(prhs[5]); // Casting double to int should be okay according to documentation.

    // Now do the calculation and write the result back.
    mwSize outputSize = (shift[1] - shift[0]) / shift_step + 1;
    plhs[0] = mxCreateNumericMatrix(1, outputSize, mxSINGLE_CLASS, mxREAL);
    float* outMatrix = (float*)mxGetData(plhs[0]);
    shifted_mutual_information(
            shift[0], shift[1],
            bin_sizes[0], bin_sizes[1],
            minmax[0], minmax[1], minmax[2], minmax[3],
            dataX, dataX + dataX_size,
            dataY, dataY + dataY_size,
            shift_step,
            outMatrix);
}
