function result = test_ishold_003
result = false;
a = figure;
surf(peaks);
hold('on');
drawnow;
result = (ishold == true);
close(a);

