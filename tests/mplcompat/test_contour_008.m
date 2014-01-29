function result = test_contour_008
result = false;
a = figure;
[Z,x,y] = peaks;
contour(x,y,Z,7,'r-');
drawnow;
close(a);
result = true;
