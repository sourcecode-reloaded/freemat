function result = test_grid_005
result = false;
a = figure;
plot(rand(1,100));
h = gca;
grid(h,'off');
drawnow;
close(a);
result = true;
