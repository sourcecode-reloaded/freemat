function result = test_clim_004
result = false;
a = figure;
image(peaks);
colorbar;
clim([0,1]);
drawnow;
close(a);
result = true;
