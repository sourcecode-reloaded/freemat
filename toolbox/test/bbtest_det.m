% Regression test function (black blox) for FreeMat v4.0.0
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_det
  bbtest_success = 1;
NumErrors = 0;
try
  A = rand(5);

catch
  NumErrors = NumErrors + 1;
end
try
  det(A)

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  B = A([2,1,3,4,5],:);

catch
  NumErrors = NumErrors + 1;
end
try
  det(B)

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
