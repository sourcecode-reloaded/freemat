function result = test_clabel_001
result = false;
a = figure;
h = contour(peaks);
clabel(h);
close(a);
result = true;
