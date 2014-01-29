function result = test_gca_001
result = false;
a = figure(1);
subplot(2,1,1);
h = gca;
drawnow;
close(a);
result = strcmp(get(h,'type'),'axes')

