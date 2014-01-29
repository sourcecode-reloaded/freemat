function result = test_clim_005
result = false;
a = figure;
surf(peaks);
colorbar;
clim([0,1]);
drawnow;
close(a);
result = true;
