function result = test_semilogx_001
result = false;
a = figure;
t = linspace(1,10);
semilogx(t,5*t.^3);
drawnow;
close(a);
result = true;
