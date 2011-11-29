% Regression test function (black box) for sum
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_elementary_sum
  bbtest_success = 1;
NumErrors = 0;
try
  A = [5,1,3;3,2,1;0,3,1]
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  sum(A)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  sum(A,2)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
