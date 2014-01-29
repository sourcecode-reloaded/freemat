function result = test_axis_002
result = false;
a = figure;
x = axis;
close(a);
result = all(x == [0 1 0 1]);

