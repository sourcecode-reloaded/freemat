function result = test_colormap_002
result = false;
a = figure;
surf(peaks);
colormap bone
drawnow
close(a);
result = true;
