function result = test_textproperties_010
result = false;
a = figure;
plot(linspace(0,1),randn(1,100));
t = text(0.5,0.5,'Hello')
drawnow;
set(t,'fontsize',15);
set(t,'fontweight','normal');
drawnow;
set(t,'fontweight','bold');
drawnow;
set(t,'fontweight','light');
drawnow;
set(t,'fontweight','demi');
drawnow;
close(a);
result = true;
