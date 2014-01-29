function result = test_ylim_003
result = false;
a = figure;
plot(rand(1,100));
ylim([.5,inf]);
drawnow
g = ylim;
drawnow;
result = all(g == [.5,1]);
close(a);
