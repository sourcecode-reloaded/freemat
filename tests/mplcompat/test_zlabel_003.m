function result = test_zlabel_003
result = false;
a = figure;
surf(peaks);
h = gca;
zlabel(h,'Test');
drawnow;
close(a);
result = true;
