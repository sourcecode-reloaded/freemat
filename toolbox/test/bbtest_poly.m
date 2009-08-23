% Regression test function (black blox) for FreeMat v4.0.0
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_poly
  bbtest_success = 1;
NumErrors = 0;
try
  A = [1,2,3;4,5,6;7,8,0]

catch
  NumErrors = NumErrors + 1;
end
try
  p = poly(A)

catch
  NumErrors = NumErrors + 1;
end
try
  r = roots(p)

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
