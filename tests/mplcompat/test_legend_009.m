function result = test_legend_009
result = false;
a = figure;
plot(rand(100,3));
legend('first','second','third','location','north');
drawnow;
close(a);
result = true;
