function result = test_text_002
result = false;
a = figure;
plot(rand(1,100));
text(50,.5,'Hello','horizontalalignment','center');
drawnow;
close(a);
result = true;
