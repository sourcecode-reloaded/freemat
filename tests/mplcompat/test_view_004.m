function result = test_view_004
result = false;
a = figure;
surf(peaks)
view(37.5,30);
drawnow;
close(a);
result = true;
