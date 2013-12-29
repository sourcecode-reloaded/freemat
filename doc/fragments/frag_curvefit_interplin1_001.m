0
xi = linspace(-4,4,100);
yi_nan = interplin1(x,y,xi,'nan');
yi_zero = interplin1(x,y,xi,'zero');
yi_endpoint = interplin1(x,y,xi,'endpoint');
yi_extrap = interplin1(x,y,xi,'extrap');
plot(x,y,'ro',xi,yi_nan,'g-x',xi,yi_zero,'g-x',xi,yi_endpoint,'g-x',xi,yi_extrap,'g-x');
mprint interplin1_2
