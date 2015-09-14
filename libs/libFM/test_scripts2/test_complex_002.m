% Test the casting of a double complex array into an integer one
function test_val = test_complex_002
scalar = 3+4i;
vec = [scalar,scalar,scalar,scalar];
iscalar = int8(scalar);
ivec = int8(vec);
test_val = isa(iscalar,'int8') & isa_fm(iscalar,'zint8') & (ivec(3) == iscalar) ...
    & isa(ivec,'int8') & isa_fm(ivec,'zint8');
