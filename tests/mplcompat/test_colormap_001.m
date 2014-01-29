function result = test_colormap_001
result = false;
a = figure;
image(peaks);
colormap bone
drawnow
close(a);
result = true;
