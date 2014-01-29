function result = test_axis_011
result = false;
a = figure;
plot(rand(1,15));
axis normal
close(a);
result = true;

