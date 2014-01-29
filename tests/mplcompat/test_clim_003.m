function result = test_clim_003
result = false;
a = figure;
surf(peaks);
x = clim;
drawnow
close(a);
result = true;
