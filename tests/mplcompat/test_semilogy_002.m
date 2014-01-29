function result = test_semilogy_002
result = false;
a = figure;
t = linspace(1,10);
semilogy(t,5*t.^3,'color','r');
drawnow;
close(a);
result = true;
