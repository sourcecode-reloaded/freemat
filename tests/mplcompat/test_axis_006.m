function result = test_axis_006
result = false;
a = figure;
plot(rand(1,15));
axis ij
close(a);
result = true;

