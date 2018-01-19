function [ ind ] = calculateIndices( data, nr_bins )
%CALCULATEINDICES Calculate indices as if the data were sorted into a
%histogram.
%   data    An array of data
%   nr_bins Number of bins if there were a histogram
% The resulting array ind has the same size as the input data.
% The indices start at 0. NaN values are mapped to nr_bins+1 and are
% therefore outside the bin range.
x = sort(data(~isnan(data)));
x_edgesi = round(linspace(1, length(x), nr_bins+1));
x_edges = x(x_edgesi);
ind = discretize(data, x_edges);

% Check if all this stuff is binned correctly.
%[~,edges,bin] = histcounts(data, x_edges);
%bin(bin == 0) = NaN;
%assert(all(x_edges == edges));
%assert(isequaln(bin, ind));
%binMinMax = minmax(ind);
%assert(binMinMax(2) - binMinMax(1) + 1 == nr_bins);

ind = ind - 1;
ind(isnan(data)) = nr_bins + 1;
end

