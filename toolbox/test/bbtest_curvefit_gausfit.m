% Regression test function (black box) for gausfit
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_curvefit_gausfit
  bbtest_success = 1;
NumErrors = 0;
try
  t = linspace(-pi,pi); 
catch
  NumErrors = NumErrors + 1;
end
try
  y = cos(t);
catch
  NumErrors = NumErrors + 1;
end
try
  [mu,sigma,dc,gain,yhat] = gausfit(t,y);
catch
  NumErrors = NumErrors + 1;
end
try
  plot(t,y,'rx',t,yhat,'g-');
catch
  NumErrors = NumErrors + 1;
end
try
  mprint gausfit1
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
