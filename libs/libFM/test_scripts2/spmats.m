a = [1,0,0,0;
     0,1,0,0,1];
b = [0,1,0,1;
     -2,3,0,0];
d = a + b*(1i);


ca = a + b*1i;
cd = b - a*1i;
A = mksparse(a);
B = mksparse(b);
C = mksparse(b');
D = mksparse(d);
CA = mksparse(ca);
CD = mksparse(cd);
CC = mksparse(cd.');