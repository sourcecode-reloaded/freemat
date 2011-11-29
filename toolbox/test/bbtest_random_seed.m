% Regression test function (black box) for seed
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_random_seed
  bbtest_success = 1;
NumErrors = 0;
try
  seed(32,41);
catch
  NumErrors = NumErrors + 1;
end
try
  rand(1,5)
catch
  NumErrors = NumErrors + 1;
end
try
  seed(32,41);
catch
  NumErrors = NumErrors + 1;
end
try
  rand(1,5)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
