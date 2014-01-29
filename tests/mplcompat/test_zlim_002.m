function result = test_zlim_002
result = false;
a = figure;
surf(peaks);
drawnow;
h = zlim;
zlim([0,3]);
g = zlim;
drawnow;
zlim('auto');
drawnow;
g = zlim;
result = all(g == h);
close(a);
