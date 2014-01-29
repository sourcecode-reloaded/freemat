function result = test_axis_004
result = false;
a = figure;
plot(rand(1,15));
axis tight
close(a);
result = true;

