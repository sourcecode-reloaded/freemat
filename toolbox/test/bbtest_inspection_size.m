% Regression test function (black box) for size
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_inspection_size
  bbtest_success = 1;
NumErrors = 0;
try
  a = randn(23,12,5);
catch
  NumErrors = NumErrors + 1;
end
try
  size(a)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  size(a,2)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
