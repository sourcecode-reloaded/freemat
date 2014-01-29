function result = test_title_004
result = false;
a = figure;
t=0:(2*pi/100):(2*pi);
x=cos(t*2).*(2+sin(t*3)*.3);
y=sin(t*2).*(2+sin(t*3)*.3);
z=cos(t*3)*.3;
tubeplot(x,y,z,0.14*sin(t*5)+.29,t,10);
drawnow;
close(a);
result = true;
