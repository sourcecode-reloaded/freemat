0
t = linspace(-1,1,256);
xmat = ones(256,1)*t; ymat = xmat';
A = exp(-(xmat.^2 + ymat.^2)*100);
image(A);
mprint('winlev1');