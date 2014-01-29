function result = test_ylim_004
result = false;
a = figure;
plot(rand(1,100));
ylim([inf,.5]);
drawnow
g = ylim;
drawnow;
result = all(g == [0,.5]);
close(a);
