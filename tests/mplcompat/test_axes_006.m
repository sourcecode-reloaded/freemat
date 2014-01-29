function result = test_axes_006
result = false;
a = figure;
h = axes('zlim',[0,10]);
draw
result = true;
close(a);
return
