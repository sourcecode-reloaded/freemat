function result = test_axis_003
result = false;
a = figure;
plot(rand(1,15));
axis auto
close(a);
result = true;

