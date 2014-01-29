function result = test_ishold_001
result = false;
a = figure;
plot(rand(1,100));
drawnow;
result = (ishold == false);
close(a);

