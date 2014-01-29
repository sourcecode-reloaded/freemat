function result = test_xlim_003
result = false;
a = figure;
plot(randn(1,100));
xlim([10,inf]);
drawnow
g = xlim;
drawnow;
result = all(g == [10,100]);
close(a);
