function result = test_imagesc_001
result = false;
a = figure;
imagesc(peaks);
drawnow;
close(a);
result = true;

