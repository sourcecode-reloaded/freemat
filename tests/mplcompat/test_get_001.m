function result = test_get_001
result = false;
a = figure(1);
b = plot(rand(1,100));
drawnow;
h = get(b,'color');
close(a);
result = true;
