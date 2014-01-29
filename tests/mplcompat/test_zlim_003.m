function result = test_zlim_003
result = false;
a = figure;
surf(peaks);
drawnow;
h = zlim;
zlim([0,inf]);
drawnow
g = zlim;
drawnow;
result = all(g == [0,h(2)]);
close(a);
