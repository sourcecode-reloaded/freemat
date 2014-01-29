function result = test_surf_004
result = false;
a = figure;
h = gca;
surf(h,peaks);
drawnow;
close(a);
result = true;
