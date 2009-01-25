% Regression test function (black blox) for FreeMat v svn
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_sec
  bbtest_success = 1;
NumErrors = 0;
try
  t = linspace(-1,1,1000);

catch
  NumErrors = NumErrors + 1;
end
try
  plot(t,sec(2*pi*t))

catch
  NumErrors = NumErrors + 1;
end
try
  axis([-1,1,-10,10]);

catch
  NumErrors = NumErrors + 1;
end
try
  mprint('secplot');

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
