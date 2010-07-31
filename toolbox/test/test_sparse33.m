function x = test_sparse33
ar = [1,0,3,4,5;6,2,3,5,0;0,0,0,0,2;0,4,5,0,3];
ai = [0,2,9,0,2;0,0,0,2,0;1,3,4,0,2;1,0,0,0,2];
a = complex(ar+ai);
A = sparse(a);
b = [3;2;5];
c = [2,4,3];
d = complex([9,3,2,4,9,0,5,4,2] + 3*i);
a(b,c) = d;
A(b,c) = d;
x = testeq(a,A);
