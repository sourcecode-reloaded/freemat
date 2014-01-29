function result = test_axes_004
result = false;
a = figure;
h = axes('xlim',[0,10]);
draw
result = true;
close(a);
return
