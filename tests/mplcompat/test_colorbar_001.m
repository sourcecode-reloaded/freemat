function result = test_colorbar_001
result = false;
a = figure;
image(peaks);
colorbar;
drawnow;
close(a);
result = true;
