% Regression test function (black box) for tand
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_mathfunctions_tand
  bbtest_success = 1;
NumErrors = 0;
try
  tand(45)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end