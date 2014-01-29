function result = test_grid_003
result = false;
a = figure;
plot(rand(1,100));
grid;
drawnow;
close(a);
result = true;
