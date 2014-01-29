function result = test_clim_007
result = false;
a = figure;
image(peaks);
colorbar;
clim auto
drawnow;
close(a);
result = true;
