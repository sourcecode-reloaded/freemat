% Regression test function (black blox) for FreeMat v4.0.0
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_gammaln
  bbtest_success = 1;
NumErrors = 0;
try
  x = linspace(0,10);

catch
  NumErrors = NumErrors + 1;
end
try
  y = gammaln(x);

catch
  NumErrors = NumErrors + 1;
end
try
  plot(x,y); xlabel('x'); ylabel('gammaln(x)');

catch
  NumErrors = NumErrors + 1;
end
try
  mprint gammaln1

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
