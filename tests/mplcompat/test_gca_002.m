function result = test_gca_001
result = false;
a = figure(1);
surf(peaks);
h = gca;
drawnow;
close(a);
result = strcmp(get(h,'type'),'axes')

