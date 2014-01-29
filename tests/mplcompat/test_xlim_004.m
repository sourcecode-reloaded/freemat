function result = test_xlim_004
result = false;
a = figure;
plot(randn(1,100));
xlim([inf,30]);
drawnow
g = xlim;
drawnow;
result = all(g == [0,30]);
close(a);
