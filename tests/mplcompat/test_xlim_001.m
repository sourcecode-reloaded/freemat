function result = test_xlim_001
result = false;
a = figure;
plot(randn(1,100));
xlim([10,30]);
drawnow;
g = xlim;
close(a);
result = all(g == [10,30]);
