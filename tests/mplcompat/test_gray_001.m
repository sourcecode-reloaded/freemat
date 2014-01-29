function result = test_gray_001
result = false;
a = figure(1);
image(peaks);
colormap('gray');
drawnow;
close(a);
result = true;
