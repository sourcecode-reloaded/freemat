function result = test_contour_001
result = false;
a = figure;
contour3(peaks);
drawnow;
close(a);
result = true;
