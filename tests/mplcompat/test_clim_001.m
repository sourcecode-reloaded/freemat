function result = test_clim_001
result = false;
a = figure;
image(peaks);
x = clim;
drawnow
close(a);
result = true;
