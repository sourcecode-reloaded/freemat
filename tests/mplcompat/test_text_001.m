function result = test_text_001
result = false;
a = figure;
plot(rand(1,100));
text(50,.5,'Hello');
drawnow;
close(a);
result = true;
