function result = test_axis_005
result = false;
a = figure;
plot(rand(1,15));
axis fill
close(a);
result = true;

