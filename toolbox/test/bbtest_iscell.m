% Regression test function (black blox) for FreeMat v4.0
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_iscell
  bbtest_success = 1;
NumErrors = 0;
try
  iscell('foo')

catch
  NumErrors = NumErrors + 1;
end
try
  iscell(2)

catch
  NumErrors = NumErrors + 1;
end
try
  iscell({1,2,3})

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
