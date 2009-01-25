function test_val = test_sscanf1
[a,b,c] = sscanf('1.34 54 hello','%g %d %s');
test_val = (a == 1.34) && (b == 54) && (c == 'hello');
