function result = test_semilogx_002
result = false;
a = figure;
t = linspace(1,10);
semilogx(t,5*t.^3,'color','r');
drawnow;
close(a);
result = true;
