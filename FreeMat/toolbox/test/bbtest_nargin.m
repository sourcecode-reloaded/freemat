% Regression test function (black blox) for FreeMat v4.0
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_nargin
  bbtest_success = 1;
NumErrors = 0;
try
  nargintest(3);

catch
  NumErrors = NumErrors + 1;
end
try
  nargintest(3,'h');

catch
  NumErrors = NumErrors + 1;
end
try
  nargintest(3,'h',1.34);

catch
  NumErrors = NumErrors + 1;
end
try
  nargintest(3,'h',1.34,pi,e);

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
