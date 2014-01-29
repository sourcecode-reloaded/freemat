function result = test_view_001
result = false;
a = figure;
plot(rand(1,100));
view(2);
drawnow;
close(a);
result = true;
