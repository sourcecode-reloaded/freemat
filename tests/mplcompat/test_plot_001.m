function result = test_plot_001
result = false;
a = figure;
t = linspace(1,10);
plot(t,5*t.^3);
drawnow;
close(a);
result = true;
