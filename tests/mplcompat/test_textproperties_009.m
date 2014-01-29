function result = test_textproperties_009
result = false;
a = figure;
plot(linspace(0,1),randn(1,100));
t = text(0.5,0.5,'Hello')
drawnow;
set(t,'fontangle','normal');
drawnow;
set(t,'fontangle','italic');
drawnow;
set(t,'fontangle','oblique');
drawnow;
close(a);
result = true;
