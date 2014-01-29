function result = test_gcf_001
result = false;
a = figure(1);
surf(peaks);
b = figure(2);
h = gcf;
drawnow;
close(a);
close(b);
result = strcmp(get(h,'type'),'figure');

