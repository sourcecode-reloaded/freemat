function result = test_plot_002
result = false;
a = figure;
t = linspace(1,10);
plot(t,5*t.^3,'r');
drawnow;
close(a);
result = true;
