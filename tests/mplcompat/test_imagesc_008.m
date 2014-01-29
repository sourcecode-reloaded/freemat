function result = test_imagesc_004
result = false;
a = figure;
imagesc(peaks,[0,5],'visible','off');
drawnow;
close(a);
result = true;

