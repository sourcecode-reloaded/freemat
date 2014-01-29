function result = test_surf_003
result = false;
a = figure;
[z,x,y] = peaks;
c = sqrt(x.^2+y.^2);
surf(x,y,z,c);
drawnow;
close(a);
result = true;
