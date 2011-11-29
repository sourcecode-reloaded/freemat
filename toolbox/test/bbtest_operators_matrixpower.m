% Regression test function (black box) for matrixpower
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_operators_matrixpower
  bbtest_success = 1;
NumErrors = 0;
try
  A = 1.5
catch
  NumErrors = NumErrors + 1;
end
try
  B = [1,.2;.2,1]
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  C = B^A
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  C = A^B
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
