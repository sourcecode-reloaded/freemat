function result = test_imagesc_004
result = false;
a = figure;
imagesc(peaks,'visible','off');
drawnow;
close(a);
result = true;

