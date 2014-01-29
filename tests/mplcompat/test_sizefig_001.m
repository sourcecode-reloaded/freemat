function result = test_sizefig_001
result = false;
a = figure;
plot(rand(100,3));
drawnow;
sizefig(500,500);
drawnow
close(a);
result = true;
