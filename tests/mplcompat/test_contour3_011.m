function result = test_contour_011
result = false;
a = figure;
h = gca;
[Z,x,y] = peaks;
contour3(h,Z,'r-');
drawnow;
close(a);
result = true;
