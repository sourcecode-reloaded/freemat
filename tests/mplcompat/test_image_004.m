function result = test_image_004
result = false;
a = figure;
image(peaks,'visible','off');
drawnow;
close(a);
result = true;

