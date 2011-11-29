% Regression test function (black box) for cond
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_array_cond
  bbtest_success = 1;
NumErrors = 0;
try
  A = [1,1;0,1e-15]
catch
  NumErrors = NumErrors + 1;
end
try
  cond(A)
catch
  NumErrors = NumErrors + 1;
end
try
  cond(A,1)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  1/rcond(A)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
