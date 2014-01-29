function result = test_view_005
result = false;
a = figure;
surf(peaks)
view(25,50);
drawnow;
close(a);
result = true;
