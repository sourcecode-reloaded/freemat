function result = test_axis_019
result = false;
a = figure;
surf(peaks);
axis tight
close(a);
result = true;

