function result = test_axes_007
result = false;
a = figure;
h = axes('xscale','log');
draw
result = true;
close(a);
return
