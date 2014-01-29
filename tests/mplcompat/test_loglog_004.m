function result = test_loglog_004
result = false;
a = figure;
h = gca;
t = linspace(1,10);
loglog(h,t,5*t.^3,'linewidth','2');
drawnow;
close(a);
result = true;
