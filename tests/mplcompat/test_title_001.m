function result = test_title_001
result = false;
a = figure;
plot(linspace(0,1),randn(1,100));
title('Test');
drawnow;
close(a);
result = true;
