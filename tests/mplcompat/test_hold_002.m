function result = test_hold_002
result = false;
a = figure;
plot(rand(1,100));
hold('off');
plot(rand(1,100));
drawnow;
close(a);
result = true;
