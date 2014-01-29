function result = test_ylim_002
result = false;
a = figure;
plot(rand(1,100));
ylim([0,.5]);
g = ylim;
drawnow;
ylim('auto');
drawnow;
g = ylim;
result = all(g == [0,1]);
close(a);
