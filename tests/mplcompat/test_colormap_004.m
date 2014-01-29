function result = test_colormap_004
result = false;
a = figure;
image(peaks);
colormap jet
drawnow
p = colormap;
close(a);
result = strcmp(p,'jet');
