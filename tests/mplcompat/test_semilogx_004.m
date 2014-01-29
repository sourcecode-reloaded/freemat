function result = test_semilogx_004
result = false;
a = figure;
h = gca;
t = linspace(1,10);
semilogx(h,t,5*t.^3,'linewidth','2');
drawnow;
close(a);
result = true;
