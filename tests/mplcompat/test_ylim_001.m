function result = test_ylim_001
result = false;
a = figure;
plot(randn(1,100));
ylim([0,1]);
drawnow;
g = ylim;
close(a);
result = all(g == [0,1]);
