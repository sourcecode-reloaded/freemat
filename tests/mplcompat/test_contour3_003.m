function result = test_contour_003
result = false;
a = figure;
contour3(peaks,[-3:.5:3]);
drawnow;
close(a);
result = true;
