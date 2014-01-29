function result = test_axis_018
result = false;
a = figure;
surf(peaks);
axis auto
close(a);
result = true;

