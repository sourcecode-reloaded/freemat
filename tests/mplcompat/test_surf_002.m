function result = test_surf_002
result = false;
a = figure;
[z,x,y] = peaks;
surf(x,y,z);
drawnow;
close(a);
result = true;

