function result = test_figure_003
result = false;
a = figure(5);
b = figure(3);
c = figure(a);
drawnow;
close(a);
close(b);
result = true;
