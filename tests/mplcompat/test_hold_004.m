function result = test_hold_004
result = false;
a = figure;
plot(rand(1,100));
h = gca;
hold(h,'on');
plot(rand(1,100));
drawnow;
close(a);
result = true;
