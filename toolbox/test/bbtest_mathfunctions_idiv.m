% Regression test function (black box) for idiv
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_mathfunctions_idiv
  bbtest_success = 1;
NumErrors = 0;
try
  idiv(27,6)
catch
  NumErrors = NumErrors + 1;
end
try
  idiv(4,-2)
catch
  NumErrors = NumErrors + 1;
end
try
  idiv(15,3)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
