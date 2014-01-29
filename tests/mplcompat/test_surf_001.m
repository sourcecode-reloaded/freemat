function result = test_surf_001
result = false;
a = figure;
h = surf(peaks);
drawnow;
close(a);
result = true;
