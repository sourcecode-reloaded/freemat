% Regression test function (black blox) for FreeMat v svn
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_isset
  bbtest_success = 1;
NumErrors = 0;
try
  who

catch
  NumErrors = NumErrors + 1;
end
try
  isset('a')

catch
  NumErrors = NumErrors + 1;
end
try
  a = [];

catch
  NumErrors = NumErrors + 1;
end
try
  isset('a')

catch
  NumErrors = NumErrors + 1;
end
try
  a = 2;

catch
  NumErrors = NumErrors + 1;
end
try
  isset('a')

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
