function result = test_ylabel_001
result = false;
a = figure;
plot(linspace(0,1),randn(1,100));
ylabel('Test $x^2$');
drawnow;
close(a);
result = true;
