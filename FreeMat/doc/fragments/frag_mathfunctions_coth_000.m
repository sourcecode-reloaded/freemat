0
x1 = -pi+.01:.01:-.01;
x2 = .01:.01:pi-.01;
plot(x1,coth(x1),x2,coth(x2)); grid('on');
mprint('cothplot');