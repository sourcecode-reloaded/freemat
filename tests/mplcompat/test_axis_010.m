function result = test_axis_010
result = false;
a = figure;
plot(rand(1,15));
axis square
close(a);
result = true;

