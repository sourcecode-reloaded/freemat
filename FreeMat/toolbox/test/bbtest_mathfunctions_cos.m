% Regression test function (black box) for cos
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_mathfunctions_cos
  bbtest_success = 1;
NumErrors = 0;
try
  x = linspace(0,1);
catch
  NumErrors = NumErrors + 1;
end
try
  plot(x,cos(2*pi*x))
catch
  NumErrors = NumErrors + 1;
end
try
  mprint('cosplot');
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end