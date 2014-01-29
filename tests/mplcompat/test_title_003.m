function result = test_title_003
result = false;
a = figure;
plot(linspace(0,1),randn(1,100));
h = gca;
title(h,'Test');
drawnow;
close(a);
result = true;
