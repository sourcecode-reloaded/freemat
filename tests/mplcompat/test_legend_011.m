function result = test_legend_010
result = false;
a = figure;
plot(rand(100,3));
legend('first','second','third','location','south');
drawnow;
close(a);
result = true;
