function result = test_pcolor_002
result = false;
a = figure;
[z,x,y] = peaks;
pcolor(x,y,z);
drawnow;
close(a);
result = true;
