function result = test_textproperties_003
result = false;
a = figure;
plot(linspace(0,1),randn(1,100));
t = text(0.5,0.5,'Hello')
drawnow
set(t,'verticalalignment','top');
drawnow
set(t,'verticalalignment','bottom');
drawnow
set(t,'verticalalignment','middle');
drawnow
close(a);
result = true;
