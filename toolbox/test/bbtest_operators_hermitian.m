% Regression test function (black box) for hermitian
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_operators_hermitian
  bbtest_success = 1;
NumErrors = 0;
try
  A = [1,2,0;4,1,-1]
catch
  NumErrors = NumErrors + 1;
end
try
  A'
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  A = [1+i,2-i]
catch
  NumErrors = NumErrors + 1;
end
try
  A.'
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
