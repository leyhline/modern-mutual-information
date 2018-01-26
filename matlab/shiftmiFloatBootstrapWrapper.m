function result = shiftmiFloatBootstrapWrapper( x, y, varargin )
%   SHIFTMIFLOATBOOTSTRAPWRAPPER Easier interface to calculating shifted mutual in parallel.
%   SHIFTMIFLOATBOOTSTRAPWRAPPER(x, y) shifts data vectors and calculates mutual
%   information at each step.
%   x A row vector holding histogram indices of the first data series.
%   y A row vector holding histogram indices of the second data series.
%   Note: Both x and y need to have the same size.
%   Optionally the following name-value pairs can be specified:
%   'shiftRange', [from to]         (default: [-500 500])
%   'binSizes',   [x-axis y-axis]   (default: [10 10])
%   'nrSamples',   scaler           (default: 100)
%   'shiftSteps',  scalar           (default: 100)
%   'shiftSteps',  stepNumber       (default: 1)
x = int32(x);
y = int32(y);
minmaxOfData = single([0 1 0 1]);  % This isn't considered but needs to be passed to the function below.
p = inputParser;
addParameter(p, 'shiftRange', [-500 500]);
addParameter(p, 'binSizes', [10 10]);
addParameter(p, 'nrSamples', 100);
addParameter(p, 'nrRepetitions', 100);
addParameter(p, 'shiftSteps', 1);
parse(p, varargin{:});
shiftRange = int32(p.Results.shiftRange);
binSizes = int32(p.Results.binSizes);
nrSamples = int32(p.Results.nrSamples);
nrRepetitions = int32(p.Results.nrRepetitions);
shiftSteps = int32(p.Results.shiftSteps);
result = shiftmiFloatBootstrap(shiftRange, binSizes, minmaxOfData, x, y, nrSamples, nrRepetitions, shiftSteps);
end

