function result = test_hist_003
result = false;
a = figure;
hist(randn(1000,1),[-1:.1:1]);
drawnow;
close(a);
result = true;
