% Regression test function (black box) for squeeze
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_array_squeeze
  bbtest_success = 1;
NumErrors = 0;
try
  x = zeros(1,4,3,1,1,2);
catch
  NumErrors = NumErrors + 1;
end
try
  size(x)
catch
  NumErrors = NumErrors + 1;
end
try
  y = squeeze(x);
catch
  NumErrors = NumErrors + 1;
end
try
  size(y)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
