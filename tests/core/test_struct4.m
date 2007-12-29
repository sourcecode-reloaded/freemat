% Test the structure constructor
function test_val = test_struct4
a = struct('foo',4,'goo',{5},'hoo',{'time',8});
test_val = test(a(1).foo == 4) & test(a(2).foo == 4) ...
    & test(a(1).goo == 5) & test(a(2).goo == 5) ...
    & test(strcmp(a(1).hoo,'time')) & test(a(2).hoo == 8);

