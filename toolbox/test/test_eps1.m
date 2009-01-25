b = eps(1.);
c = eps('single');
d = eps(single(1.));
test_val = test(a==b && c==d);
