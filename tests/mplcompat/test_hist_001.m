function result = test_hist_001
result = false;
a = figure;
hist(randn(1000,1));
drawnow;
close(a);
result = true;
