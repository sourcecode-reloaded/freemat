function result = test_subplot_003
result = false;
a = figure;
subplot(4,4,[3,4,7,8]);
drawnow;
close(a);
result = true;
