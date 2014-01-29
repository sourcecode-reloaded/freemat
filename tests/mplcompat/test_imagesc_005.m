function result = test_imagesc_005
result = false;
a = figure;
imagesc(peaks,[0,5]);
drawnow;
close(a);
result = true;

