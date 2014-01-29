function result = test_xlabel_002
result = false;
a = figure;
plot(linspace(0,1),randn(1,100));
xlabel('Test');
drawnow;
close(a);
result = true;
