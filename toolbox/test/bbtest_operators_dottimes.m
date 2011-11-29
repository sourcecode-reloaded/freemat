% Regression test function (black box) for dottimes
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_operators_dottimes
  bbtest_success = 1;
NumErrors = 0;
try
  3 .* 8
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  3.1 .* [2,4,5,6,7]
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  a = 3 + 4*i
catch
  NumErrors = NumErrors + 1;
end
try
  b = a .* 2
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  a = [1,2;3,4]
catch
  NumErrors = NumErrors + 1;
end
try
  b = [2,3;6,7]
catch
  NumErrors = NumErrors + 1;
end
try
  c = a .* b
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
