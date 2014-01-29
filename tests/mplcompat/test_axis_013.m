function result = test_axis_013
result = false;
a = figure;
plot(rand(1,15));
axis on
close(a);
result = true;

