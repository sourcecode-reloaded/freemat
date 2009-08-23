% Regression test function (black blox) for FreeMat v4.0.0
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_shiftdim
  bbtest_success = 1;
NumErrors = 0;
try
  x = uint8(10*randn(1,1,1,3,2));

catch
  NumErrors = NumErrors + 1;
end
try
  [y,n] = shiftdim(x);

catch
  NumErrors = NumErrors + 1;
end
try
  n

catch
  NumErrors = NumErrors + 1;
end
try
  size(y)

catch
  NumErrors = NumErrors + 1;
end
try
  c = shiftdim(y,-n);

catch
  NumErrors = NumErrors + 1;
end
try
  size(c)

catch
  NumErrors = NumErrors + 1;
end
try
  any(c~=x)

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  z = shiftdim(x,4);

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  squeeze(x)

catch
  NumErrors = NumErrors + 1;
end
try
  squeeze(z)

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
