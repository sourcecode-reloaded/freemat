function result = test_plot_003
result = false;
a = figure;
t = linspace(1,10);
plot(t,5*t.^3,'r',t,4*t);
drawnow;
close(a);
result = true;
