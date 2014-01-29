function result = test_get_002
result = false;
a = figure(1);
b = plot(rand(1,100));
drawnow;
h = get(b,'visible');
close(a);
result = true;
