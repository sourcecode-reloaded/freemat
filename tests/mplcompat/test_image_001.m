function result = test_image_001
result = false;
a = figure;
image(peaks);
drawnow;
close(a);
result = true;

