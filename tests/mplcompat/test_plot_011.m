function result = test_plot_011
result = false;
a = figure;
h = gca;
plot(h,rand(100,1),'ko:',randn(100,1),'b<--')
drawnow;
close(a);
result = true;
