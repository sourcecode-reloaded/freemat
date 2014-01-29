function result = test_imagesc_002
result = false;
a = figure;
imagesc(peaks,'xdata',[0,100]);
drawnow;
close(a);
result = true;

