% Regression test function (black box) for imagesc
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_handle_imagesc
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
  imagesc(x,[0 .5])
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
