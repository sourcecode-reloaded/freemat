function result = test_contour_010
result = false;
a = figure;
[Z,x,y] = peaks;
contour3(Z,'r-');
drawnow;
close(a);
result = true;
