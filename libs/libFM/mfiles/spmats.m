a = [1,0,0,0;
     0,1,0,0,1];
b = [0,1,0,1;
     -2,3,0,0];

ca = a + b*1i;
cd = b - a*1i;
A = mksparse(a);
B = mksparse(b);
CA = mksparse(ca);
CD = mksparse(cd);
