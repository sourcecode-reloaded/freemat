function result = test_axis_007
result = false;
a = figure;
plot(rand(1,15));
axis xy
close(a);
result = true;

