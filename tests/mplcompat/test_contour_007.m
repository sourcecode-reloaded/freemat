function result = test_contour_007
result = false;
a = figure;
[Z,x,y] = peaks;
contour(x,y,Z,[-3:.5:3],'r-');
drawnow;
close(a);
result = true;
