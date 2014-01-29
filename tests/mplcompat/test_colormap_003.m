function result = test_colormap_003
result = false;
a = figure;
image(peaks);
colormap(rand(128,3))
drawnow
close(a);
result = true;
