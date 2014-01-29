function result = test_clabel_002
result = false;
a = figure;
h = contour(peaks);
clabel(h,[-2.5,0,5]);
close(a);
result = true;
