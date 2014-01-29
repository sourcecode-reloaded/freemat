function result = test_pcolor_001
result = false;
a = figure;
pcolor(peaks);
drawnow;
close(a);
result = true;
