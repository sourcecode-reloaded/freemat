% Regression test function (black box) for asecd
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_mathfunctions_asecd
  bbtest_success = 1;
NumErrors = 0;
try
  asecd(2/sqrt(2))
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  asecd(2)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
