function result = test_legend_001
result = false;
a = figure;
plot(rand(100,3));
legend('first','second','third');
drawnow;
close(a);
result = true;
