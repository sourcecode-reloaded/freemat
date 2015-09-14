% Test the assignment of a complex value into a real valued array
function test_val = test_complex_001
a = 1:5;
a(3) = 3+4i;
test_val = isa(a,'double') & isa_fm(a,'zdouble') & (a(3) == 3+4i);


