function result = test_zlabel_002
result = false;
a = figure;
surf(peaks);
zlabel('Test');
drawnow;
close(a);
result = true;
