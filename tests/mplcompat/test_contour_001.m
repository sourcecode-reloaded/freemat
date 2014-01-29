function result = test_contour_001
result = false;
a = figure;
contour(peaks);
drawnow;
close(a);
result = true;
