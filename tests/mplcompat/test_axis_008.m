function result = test_axis_008
result = false;
a = figure;
plot(rand(1,15));
axis equal
close(a);
result = true;

