function result = test_axes_013
result = false;
a = figure;
h = axes('xdir','reverse');
draw
result = true;
close(a);
return
