% Regression test function (black blox) for FreeMat v4.0
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_acot
  bbtest_success = 1;
NumErrors = 0;
try
  x1 = -2*pi:pi/30:-0.1;

catch
  NumErrors = NumErrors + 1;
end
try
  x2 = 0.1:pi/30:2*pi;

catch
  NumErrors = NumErrors + 1;
end
try
  plot(x1,acot(x1),x2,acot(x2)); grid('on');

catch
  NumErrors = NumErrors + 1;
end
try
  mprint('acotplot');

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
