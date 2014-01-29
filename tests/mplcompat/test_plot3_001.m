function result = test_plot3_001
result = false;
a = figure;
t = linspace(0,5*pi,200);
plot3(cos(t),sin(t),t);
drawnow;
close(a);
result = true;
