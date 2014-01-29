function result = test_grid_004
result = false;
a = figure;
plot(rand(1,100));
h = gca;
grid(h,'on');
drawnow;
close(a);
result = true;
