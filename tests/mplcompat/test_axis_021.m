function result = test_axis_021
result = false;
a = figure;
surf(peaks);
axis ij
close(a);
result = true;

