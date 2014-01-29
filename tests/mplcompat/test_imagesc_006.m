function result = test_imagesc_006
result = false;
a = figure;
imagesc(peaks,[0,5],'xdata',[0,100]);
drawnow;
close(a);
result = true;

