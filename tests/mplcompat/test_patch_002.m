function result = test_patch_002
result = false;
a = figure;
x = [0,1,0,1]'; x = [x;x];
y = [0,0,1,1]'; y = [y;y];
z = [-.5,-.5,-.5,-.5,.5,.5,.5,.5]';
c = [1,1,1];
patch(x,y,z,'r');
drawnow;
close(a);
result = true;
