function result = test_contour_013
result = false;
a = figure;
h = gca;
[Z,x,y] = peaks;
contour3(h,x,y,Z,'r-');
drawnow;
close(a);
result = true;
