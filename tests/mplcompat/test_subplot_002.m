function result = test_subplot_002
result = false;
a = figure;
subplot 221
drawnow;
close(a);
result = true;
