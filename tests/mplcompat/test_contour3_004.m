function result = test_contour_004
result = false;
a = figure;
[Z,x,y] = peaks;
contour3(x,y,Z);
drawnow;
close(a);
result = true;
