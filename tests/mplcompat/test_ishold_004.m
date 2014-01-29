function result = test_ishold_004
result = false;
a = figure;
plot(rand(1,100));
hold('on');
drawnow;
result = (ishold == true);
close(a);
