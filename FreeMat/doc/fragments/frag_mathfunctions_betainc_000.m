0
x=.2:.01:.8;
y = betainc(x,5,3);
plot(x,y); xlabel('x'); ylabel('betainc(x,5,3)');
mprint betainc1
