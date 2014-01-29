function result = test_grid_001
result = false;
a = figure;
plot(rand(1,100));
grid('on');
drawnow;
close(a);
result = true;
