function result = test_contour_003
result = false;
a = figure;
contour(peaks,7);
drawnow;
close(a);
result = true;
