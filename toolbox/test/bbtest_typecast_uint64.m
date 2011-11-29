% Regression test function (black box) for uint64
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_typecast_uint64
  bbtest_success = 1;
NumErrors = 0;
try
  uint64(200)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  uint64(40e9)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  uint64(-100)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  uint64(pi)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  uint64(5+2*i)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  uint64('helo')
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  uint64({4})
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 1) bbtest_success = 0; return; end
