function result = test_hist_004
result = false;
a = figure;
hist(randn(1000,1),[-1:.1:1],'normed',true);
drawnow;
close(a);
result = true;
