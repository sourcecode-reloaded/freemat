function result = test_hold_006
result = false;
a = figure;
plot(rand(1,100));
h = gca;
hold(h);
plot(rand(1,100));
drawnow;
close(a);
result = true;
