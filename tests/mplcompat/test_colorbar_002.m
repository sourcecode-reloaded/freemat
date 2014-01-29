function result = test_colorbar_002
result = false;
a = figure;
surf(peaks);
colorbar;
drawnow;
close(a);
result = true;
