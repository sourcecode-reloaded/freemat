function result = test_plot_006
result = false;
a = figure;
h = gca;
plot(h,rand(100,3),'r-')
drawnow;
close(a);
result = true;
