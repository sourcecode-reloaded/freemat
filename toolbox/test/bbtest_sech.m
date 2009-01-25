% Regression test function (black blox) for FreeMat v svn
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_sech
  bbtest_success = 1;
NumErrors = 0;
try
  x = -2*pi:.01:2*pi;

catch
  NumErrors = NumErrors + 1;
end
try
  plot(x,sech(x)); grid('on');

catch
  NumErrors = NumErrors + 1;
end
try
  mprint('sechplot');

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
