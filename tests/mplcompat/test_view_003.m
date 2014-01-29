function result = test_view_003
result = false;
a = figure;
surf(peaks)
view(3);
drawnow;
close(a);
result = true;
