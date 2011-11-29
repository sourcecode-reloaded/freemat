% Regression test function (black box) for sprand
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_sparse_sprand
  bbtest_success = 1;
NumErrors = 0;
try
  x = [1,0,0;0,0,1;1,0,0]
catch
  NumErrors = NumErrors + 1;
end
try
  y = sprand(x)
catch
  NumErrors = NumErrors + 1;
end
try
  full(y)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  y = sprand(10000,10000,.001);
catch
  NumErrors = NumErrors + 1;
end
try
  nnz(y)/10000^2
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
