% Regression test function (black box) for rcond
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_array_rcond
  bbtest_success = 1;
NumErrors = 0;
try
  A = rand(30);
catch
  NumErrors = NumErrors + 1;
end
try
  rcond(A)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  1/(norm(A,1)*norm(inv(A),1))
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
