function result = test_hold_003
result = false;
a = figure;
plot(rand(1,100));
hold;
plot(rand(1,100));
drawnow;
close(a);
result = true;
