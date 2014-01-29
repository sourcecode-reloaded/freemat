function result = test_axes_013
result = false;
a = figure;
h = axes('ydir','reverse');
draw
result = true;
close(a);
return
