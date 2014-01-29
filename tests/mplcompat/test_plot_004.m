function result = test_plot_004
result = false;
a = figure;
t = linspace(1,10);
plot(t,5*t.^3,'r',t,4*t,'linewidth',2);
drawnow;
close(a);
result = true;
