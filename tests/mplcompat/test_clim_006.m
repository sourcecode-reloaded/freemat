function result = test_clim_006
result = false;
a = figure;
surf(peaks);
colorbar;
clim auto
drawnow;
close(a);
result = true;
