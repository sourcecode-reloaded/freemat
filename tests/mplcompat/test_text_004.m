function result = test_text_003
result = false;
a = figure;
plot(rand(1,100));
text([25,50,75],[.5,.5,.5],{'Hello','there','people'},'horizontalalignment','center');
drawnow;
close(a);
result = true;
