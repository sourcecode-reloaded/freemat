% Regression test function (black box) for gray
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_handle_gray
  bbtest_success = 1;
NumErrors = 0;
try
  x = linspace(-1,1,512)'*ones(1,512);
catch
  NumErrors = NumErrors + 1;
end
try
  y = x';
catch
  NumErrors = NumErrors + 1;
end
try
  Z = exp(-(x.^2+y.^2)/0.3);
catch
  NumErrors = NumErrors + 1;
end
try
  image(Z);
catch
  NumErrors = NumErrors + 1;
end
try
  colormap(gray);
catch
  NumErrors = NumErrors + 1;
end
try
  mprint gray1
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
