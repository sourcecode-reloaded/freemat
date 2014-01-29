function result = test_axis_009
result = false;
a = figure;
plot(rand(1,15));
axis image
close(a);
result = true;

