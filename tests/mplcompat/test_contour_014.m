function result = test_contour_014
result = false;
a = figure;
h = gca;
[Z,x,y] = peaks;
contour(h,x,y,Z,7,'r-');
drawnow;
close(a);
result = true;
