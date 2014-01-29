function result = test_contour_015
result = false;
a = figure;
h = gca;
[Z,x,y] = peaks;
contour(h,x,y,Z,[-3:.5:3],'r-');
drawnow;
close(a);
result = true;
