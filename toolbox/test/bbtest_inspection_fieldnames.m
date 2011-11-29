% Regression test function (black box) for fieldnames
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_inspection_fieldnames
  bbtest_success = 1;
NumErrors = 0;
try
  y.foo = 3; y.goo = 'hello';
catch
  NumErrors = NumErrors + 1;
end
try
  x = fieldnames(y)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
