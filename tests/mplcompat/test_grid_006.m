function result = test_grid_006
result = false;
a = figure;
plot(rand(1,100));
h = gca;
grid(h);
drawnow;
close(a);
result = true;
