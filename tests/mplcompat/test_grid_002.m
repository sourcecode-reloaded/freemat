function result = test_grid_002
result = false;
a = figure;
plot(rand(1,100));
grid('off');
drawnow;
close(a);
result = true;
