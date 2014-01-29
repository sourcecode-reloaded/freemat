function result = test_imagesc_003
result = false;
a = figure;
imagesc(peaks,'ydata',[0,100]);
drawnow;
close(a);
result = true;

