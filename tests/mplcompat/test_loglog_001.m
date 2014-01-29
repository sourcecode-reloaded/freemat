function result = test_loglog_001
result = false;
a = figure;
t = linspace(1,10);
loglog(t,5*t.^3);
drawnow;
close(a);
result = true;
