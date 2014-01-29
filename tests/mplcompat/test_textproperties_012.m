function result = test_textproperties_012
result = false;
a = figure;
plot(linspace(0,1),randn(1,100));
t = text(0.5,0.5,'Hello')
drawnow;
set(t,'visible','off');
drawnow;
close(a);
result = true;
