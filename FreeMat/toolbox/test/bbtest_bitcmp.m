% Regression test function (black blox) for FreeMat v4.0
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_bitcmp
  bbtest_success = 1;
NumErrors = 0;
try
  bitcmp(uint16(2^14-2))

catch
  NumErrors = NumErrors + 1;
end
try
  bitcmp(uint16(2^14-2),14)

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
