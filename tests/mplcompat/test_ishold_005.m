function result = test_ishold_005
result = false;
a = figure;
plot(rand(1,100));
hold('on');
drawnow;
hold('off');
drawnow;
result = (ishold == false);
close(a);
