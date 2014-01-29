function result = test_subplot_001
result = false;
a = figure;
subplot(2,2,1);
drawnow;
close(a);
result = true;
