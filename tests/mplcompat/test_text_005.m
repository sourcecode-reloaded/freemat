function result = test_text_005
result = false;
a = figure;
plot(rand(1,100));
text([25,50,75],[.5,.5,.5],{'Hello','there','people'});
drawnow;
close(a);
result = true;
