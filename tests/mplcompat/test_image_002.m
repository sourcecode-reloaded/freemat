function result = test_image_002
result = false;
a = figure;
image(peaks,'xdata',[0,100]);
drawnow;
close(a);
result = true;

