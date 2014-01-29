function result = test_textproperties_006
result = false;
a = figure;
plot(linspace(0,1),randn(1,100));
t = text(0.5,0.5,'Hello')
drawnow
set(t,'edgecolor','k');
set(t,'linewidth',2);
drawnow
close(a);
result = true;
