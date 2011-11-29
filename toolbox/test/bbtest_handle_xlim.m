% Regression test function (black box) for xlim
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_handle_xlim
  bbtest_success = 1;
NumErrors = 0;
try
  x = linspace(-1,1);
catch
  NumErrors = NumErrors + 1;
end
try
  y = sin(2*pi*x);
catch
  NumErrors = NumErrors + 1;
end
try
  plot(x,y,'r-');
catch
  NumErrors = NumErrors + 1;
end
try
  xlim  % what are the current limits?
catch
  NumErrors = NumErrors + 1;
end
try
  mprint xlim1
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  plot(x,y,'r-')
catch
  NumErrors = NumErrors + 1;
end
try
  xlim([-0.2,0.2])
catch
  NumErrors = NumErrors + 1;
end
try
  mprint xlim2
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  plot(x,y,'r-');
catch
  NumErrors = NumErrors + 1;
end
try
  xlim([0,inf])
catch
  NumErrors = NumErrors + 1;
end
try
  mprint xlim3
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
