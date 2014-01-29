function result = test_surf_006
result = false;
a = figure;
h = gca;
[z,x,y] = peaks;
c = sqrt(x.^2+y.^2);
surf(h,x,y,z,c);
drawnow;
close(a);
result = true;
