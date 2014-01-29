function result = test_set_002
result = false;
a = figure(1);
b = plot(rand(1,100));
drawnow;
h = set(b,'visible','off');
close(a);
result = true;
