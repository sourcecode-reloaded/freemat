function result = test_image_003
result = false;
a = figure;
image(peaks,'ydata',[0,100]);
drawnow;
close(a);
result = true;

