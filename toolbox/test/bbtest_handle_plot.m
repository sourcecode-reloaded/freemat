% Regression test function (black box) for plot
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_handle_plot
  bbtest_success = 1;
NumErrors = 0;
try
  x = linspace(-pi,pi);
catch
  NumErrors = NumErrors + 1;
end
try
  y = cos(x);
catch
  NumErrors = NumErrors + 1;
end
try
  plot(x,y,'r-');
catch
  NumErrors = NumErrors + 1;
end
try
  mprint plot1
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  x = linspace(-pi,pi);
catch
  NumErrors = NumErrors + 1;
end
try
  y = [cos(x(:)),cos(3*x(:)),cos(5*x(:))];
catch
  NumErrors = NumErrors + 1;
end
try
  plot(x,y);
catch
  NumErrors = NumErrors + 1;
end
try
  mprint plot2
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  plot(x,y(:,1),'rx-',x,y(:,2),'b>-',x,y(:,3),'g*:');
catch
  NumErrors = NumErrors + 1;
end
try
  mprint plot3
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  plot(y(:,1),'r:',y(:,2),'b;',y(:,3),'g|');
catch
  NumErrors = NumErrors + 1;
end
try
  mprint plot4
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  y = cos(2*x) + i * cos(3*x);
catch
  NumErrors = NumErrors + 1;
end
try
  plot(y);
catch
  NumErrors = NumErrors + 1;
end
try
  mprint plot5
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  t = linspace(-3,3);
catch
  NumErrors = NumErrors + 1;
end
try
  plot(cos(5*t).*exp(-t),'r-','linewidth',3);
catch
  NumErrors = NumErrors + 1;
end
try
  mprint plot6
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
