% Regression test function (black blox) for FreeMat v4.0.0
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_sprintf
  bbtest_success = 1;
NumErrors = 0;
try
  l = {}; for i = 1:5; s = sprintf('file_%d.dat',i); l(i) = {s}; end;

catch
  NumErrors = NumErrors + 1;
end
try
  l

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
