function result = test_plot_010
result = false;
a = figure;
h = gca;
plot(h,rand(100,3),'r-',randn(100,3))
drawnow;
close(a);
result = true;
