function result = test_plot_006
result = false;
a = figure;
h = gca;
t = linspace(1,10);
plot(h,t+i*t,'r',t,4*t,'linewidth',2);
drawnow;
close(a);
result = true;
