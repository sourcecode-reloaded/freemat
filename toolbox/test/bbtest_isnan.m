% Regression test function (black blox) for FreeMat v4.0.0
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_isnan
  bbtest_success = 1;
NumErrors = 0;
try
  a = [1.2 3.4 nan 5]

catch
  NumErrors = NumErrors + 1;
end
try
  isnan(a)

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
