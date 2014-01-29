function result = test_contour_009
result = false;
a = figure;
[Z,x,y] = peaks;
contour3(x,y,Z,'r-');
drawnow;
close(a);
result = true;
