function result = test_figure_004
result = false;
a = figure(5);
set(a,'color','b');
drawnow;
close(a);
result = true;
