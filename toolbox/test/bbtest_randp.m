% Regression test function (black blox) for FreeMat v4.0
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_randp
  bbtest_success = 1;
NumErrors = 0;
try
  randp(33*ones(1,10))

catch
  NumErrors = NumErrors + 1;
end
try
  randbin(1000*ones(1,10),33/1000*ones(1,10))

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
