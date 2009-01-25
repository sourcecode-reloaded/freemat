% Regression test function (black blox) for FreeMat v svn
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_acosd
  bbtest_success = 1;
NumErrors = 0;
try
  acosd(sqrt(2)/2)

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  acosd(0.5)

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
