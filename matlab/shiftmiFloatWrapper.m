function result = shiftmiFloatWrapper( x, y, minmaxOfData, varargin )
%   SHIFTMIFLOATWRAPPER Easier interface to calculating shifted mutual in parallel.
%   SHIFTMIFLOATWRAPPER(x, y) shifts data vectors and calculates mutual
%   information at each step.
%   x A row vector holding histogram indices of the first data series.
%   y A row vector holding histogram indices of the second data series.
%   minmaxOfData A row vector holding the minimum and maximum values
%                of the data: [minFirst maxFirst minSecond maxSecond]
%   Note: Both x and y need to have the same size.
%   Optionally the following name-value pairs can be specified:
%   'shiftRange', [from to]         (default: [-500 500])
%   'binSizes',   [x-axis y-axis]   (default: [10 10])
%   'shiftSteps',  stepNumber       (default: 1)
x = int32(x);
y = int32(y);
minmaxOfData = single(minmaxOfData);
p = inputParser;
addParameter(p, 'shiftRange', [-500 500]);
addParameter(p, 'binSizes', [10 10]);
addParameter(p, 'shiftSteps', 1);
parse(p, varargin{:});
shiftRange = int32(p.Results.shiftRange);
binSizes = int32(p.Results.binSizes);
shiftSteps = int32(p.Results.shiftSteps);
result = shiftmiFloat(shiftRange, binSizes, minmaxOfData, x, y, shiftSteps);
end

