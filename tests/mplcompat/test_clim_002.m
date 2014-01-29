function result = test_clim_002
result = false;
a = figure;
image(peaks);
clim auto
drawnow
close(a);
result = true;
