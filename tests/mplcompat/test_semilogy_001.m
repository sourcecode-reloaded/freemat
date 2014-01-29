function result = test_semilogy_001
result = false;
a = figure;
t = linspace(1,10);
semilogy(t,5*t.^3);
drawnow;
close(a);
result = true;
