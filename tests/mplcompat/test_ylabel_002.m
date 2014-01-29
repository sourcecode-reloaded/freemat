function result = test_ylabel_002
result = false;
a = figure;
plot(linspace(0,1),randn(1,100));
ylabel('Test');
drawnow;
close(a);
result = true;
