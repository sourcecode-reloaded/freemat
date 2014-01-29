function result = test_zlim_004
result = false;
a = figure;
surf(peaks);
drawnow;
h = zlim;
zlim([inf,0]);
drawnow
g = zlim;
drawnow;
result = all(g == [h(1),0]);
close(a);
