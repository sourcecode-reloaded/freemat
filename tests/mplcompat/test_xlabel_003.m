function result = test_xlabel_003
result = false;
a = figure;
plot(linspace(0,1),randn(1,100));
h = gca;
xlabel(h,'Test');
drawnow;
close(a);
result = true;
