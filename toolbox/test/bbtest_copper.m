% Regression test function (black blox) for FreeMat v4.0.0
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_copper
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
  colormap(copper);

catch
  NumErrors = NumErrors + 1;
end
try
  mprint copper1

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
