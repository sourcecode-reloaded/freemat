function result = test_axes_009
result = false;
a = figure;
h = axes('xticklabel','1|2|3|4|5');
draw
result = true;
close(a);
return
