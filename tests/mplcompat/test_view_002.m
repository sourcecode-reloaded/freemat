function result = test_view_002
result = false;
a = figure;
surf(peaks)
view(2);
drawnow;
close(a);
result = true;
