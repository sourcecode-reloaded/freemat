function result = test_xlabel_001
result = false;
a = figure;
plot(linspace(0,1),randn(1,100));
xlabel('Test $x^2$');
drawnow;
close(a);
result = true;
