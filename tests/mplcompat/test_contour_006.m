function result = test_contour_006
result = false;
a = figure;
[Z,x,y] = peaks;
contour(x,y,Z,[-3:.5:3]);
drawnow;
close(a);
result = true;
