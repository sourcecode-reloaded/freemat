function result = test_hist_002
result = false;
a = figure;
hist(randn(1000,1),50);
drawnow;
close(a);
result = true;
