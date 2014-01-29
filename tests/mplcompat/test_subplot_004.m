function result = test_subplot_004
result = false;
a = figure;
subplot(2,2,1);
plot(rand(1,100));
subplot(2,2,2);
image(peaks);
subplot(2,2,3);
surf(peaks);
subplot(2,2,4);
plot(rand(1,100));
drawnow;
close(a);
result = true;
