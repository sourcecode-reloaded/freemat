function result = test_set_001
result = false;
a = figure(1);
b = plot(rand(1,100));
drawnow;
h = set(b,'color','b');
close(a);
result = true;
