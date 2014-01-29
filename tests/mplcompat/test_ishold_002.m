function result = test_ishold_002
result = false;
a = figure;
surf(peaks);
drawnow;
result = (ishold == false);
close(a);

