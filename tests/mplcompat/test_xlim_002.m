function result = test_xlim_002
result = false;
a = figure;
plot(randn(1,100));
xlim([10,30]);
g = xlim;
drawnow;
xlim('auto');
drawnow;
g = xlim;
result = all(g == [0,100]);
close(a);
