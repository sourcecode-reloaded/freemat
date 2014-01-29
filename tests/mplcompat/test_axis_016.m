function result = test_axis_016
result = false;
a = figure;
h = gca;
axis(h,[0 1 0 1])
close(a);
result = true;
