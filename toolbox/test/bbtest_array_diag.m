% Regression test function (black box) for diag
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_array_diag
  bbtest_success = 1;
NumErrors = 0;
try
  A = int32(10*rand(4,5))
catch
  NumErrors = NumErrors + 1;
end
try
  diag(A)
catch
  NumErrors = NumErrors + 1;
end
try
  diag(A,1)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  x = int32(10*rand(1,3))
catch
  NumErrors = NumErrors + 1;
end
try
  diag(x)
catch
  NumErrors = NumErrors + 1;
end
try
  diag(x,-1)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
