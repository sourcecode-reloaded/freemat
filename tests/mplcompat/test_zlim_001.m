function result = test_zlim_001
result = false;
a = figure;
surf(peaks);
zlim([0,3]);
drawnow;
g = zlim;
close(a);
result = all(g == [0,3]);
