% Check that matrix cat works properly with spaces before the continuation
function test_val = test_matcat7
a = [1;2;...   
     3;4; ...
     5;6];
b = [1, 2, 3, ...
     4, 5, 6 ];
c=[1, 2, ...
              3];
test_val = 1;
