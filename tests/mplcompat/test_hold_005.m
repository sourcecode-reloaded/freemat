function result = test_hold_005
result = false;
a = figure;
plot(rand(1,100));
h = gca;
hold(h,'off');
plot(rand(1,100));
drawnow;
close(a);
result = true;
