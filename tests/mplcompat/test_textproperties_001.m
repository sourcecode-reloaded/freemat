function result = test_textproperties_001
result = false;
a = figure;
plot(linspace(0,1),randn(1,100));
t = text(0.5,0.5,'Hello')
drawnow
set(t,'horizontalalignment','left');
drawnow
set(t,'horizontalalignment','center');
drawnow
set(t,'horizontalalignment','right');
drawnow
close(a);
result = true;
