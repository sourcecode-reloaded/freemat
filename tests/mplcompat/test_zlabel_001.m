function result = test_zlabel_001
result = false;
a = figure;
surf(peaks);
zlabel('Test $x^2$');
drawnow;
close(a);
result = true;
