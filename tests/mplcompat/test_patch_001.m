function result = test_patch_001
result = false;
a = figure;
x = [0,1,0,1]';
y = [0,0,1,1]';
c = [1,1,1];
patch(x,y,'r');
drawnow;
close(a);
result = true;
