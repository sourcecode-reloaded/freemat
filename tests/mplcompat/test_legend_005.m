function result = test_legend_002
result = false;
a = figure;
plot(rand(100,3));
legend('first','second','third');
drawnow;
legend('boxoff');
drawnow;
close(a);
result = true;
