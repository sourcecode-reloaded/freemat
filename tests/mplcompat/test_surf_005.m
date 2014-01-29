function result = test_surf_002
result = false;
a = figure;
h = gca;
[z,x,y] = peaks;
surf(h,x,y,z);
drawnow;
close(a);
result = true;

