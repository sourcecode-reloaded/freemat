% Regression test function (black box) for image
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_handle_image
  bbtest_success = 1;
NumErrors = 0;
try
  x = rand(512);
catch
  NumErrors = NumErrors + 1;
end
try
  x((-64:63)+256,(-128:127)+256) = 1.0;
catch
  NumErrors = NumErrors + 1;
end
try
  figure
catch
  NumErrors = NumErrors + 1;
end
try
  image(x)
catch
  NumErrors = NumErrors + 1;
end
try
  colormap(gray)
catch
  NumErrors = NumErrors + 1;
end
try
  mprint image1
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  t = linspace(0,1);
catch
  NumErrors = NumErrors + 1;
end
try
  red = t'*t;
catch
  NumErrors = NumErrors + 1;
end
try
  green = t'*(t.^2);
catch
  NumErrors = NumErrors + 1;
end
try
  blue = t'*(0*t+1);
catch
  NumErrors = NumErrors + 1;
end
try
  A(:,:,1) = red; 
catch
  NumErrors = NumErrors + 1;
end
try
  A(:,:,2) = green; 
catch
  NumErrors = NumErrors + 1;
end
try
  A(:,:,3) = blue;
catch
  NumErrors = NumErrors + 1;
end
try
  image(A);
catch
  NumErrors = NumErrors + 1;
end
try
  mprint image2
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
