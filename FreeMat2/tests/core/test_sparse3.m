function x = test_sparse3
a = float([0,0,2,0;0,0,1,1;1,0,0,0]);
b = float([4,5,0,3]);
c = [0,0];
d = [2,3];
A = sparse(a);
B = sparse(b);
C = sparse(c);
D = sparse(d);
j = [a;c,d;b];
J = [A;C,D;B];
x = testeq(j,J);

