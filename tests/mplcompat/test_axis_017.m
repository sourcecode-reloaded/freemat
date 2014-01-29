function result = test_axis_017
result = false;
a = figure;
x = axis(gca);
close(a);
result = all(x == [0 1 0 1]);

