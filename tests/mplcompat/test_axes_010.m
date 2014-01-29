function result = test_axes_010
result = false;
a = figure;
h = axes('yticklabel','1|2|3|4|5');
draw
result = true;
close(a);
return
